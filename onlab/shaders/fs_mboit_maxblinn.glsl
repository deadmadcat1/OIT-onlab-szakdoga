#version 460
precision highp float;

uniform int	nLights;
uniform struct {
	vec4 pos;
	vec3 La;
	vec3 Le;
} lights[8];

uniform struct {
	vec3 kd, ks, ka;
	float alpha, shine;
} material;

uniform float wrapping_zone_param;

uniform sampler2D depthSampler;

uniform sampler2D totalAbsorbance;
uniform sampler2D moment1;
uniform sampler2D moment23;

in vec3 wLightDir[8];
in vec4 wPos;
in vec3 wNormal;
in vec3 wView;

layout(location = 0) out vec4 transparentTarget;

vec2 c_conj(vec2 Z){
	return vec2(Z.x,-Z.y);
}

vec2 c_mul(vec2 lhs, vec2 rhs){
	return vec2(lhs.x*rhs.x-lhs.y*rhs.y,lhs.x*rhs.y+lhs.y*rhs.x);
}

vec2 c_div(vec2 num,vec2 denom){
	return vec2(num.x*denom.x+num.y*denom.y,-num.x*denom.y+num.y*denom.x)/dot(denom,denom);
}

vec2 c_div(float num,vec2 denom){
	return vec2(num*denom.x,-num*denom.y)/dot(denom,denom);
}

vec2 c_recip(vec2 Z){
	return vec2(Z.x,-Z.y)/dot(Z,Z);
}

vec2 c_pow2(vec2 Z){
	return vec2(Z.x * Z.x - Z.y * Z.y, 2.0f * Z.x * Z.y);
}

vec2 c_pow3(vec2 Z){
	return c_mul(c_pow2(Z), Z);
}

/* This function is continuous but not defined on the negative real 
			axis (and cannot be continued continuously there).
*/
vec2 c_sqrtUnsafe(vec2 Z){
	float ZLengthSq = dot(Z, Z);
	float ZLengthInv = 1.0f / sqrt(ZLengthSq);
	vec2 UnnormalizedRoot = Z * ZLengthInv + vec2(1.0f, 0.0f);
	float UnnormalizedRootLengthSq = dot(UnnormalizedRoot, UnnormalizedRoot);
	float NormalizationFactorInvSq = UnnormalizedRootLengthSq * ZLengthInv;
	float NormalizationFactor = 1.0f / sqrt(NormalizationFactorInvSq);
	return NormalizationFactor * UnnormalizedRoot;
}
/*
  This function has discontinuities for values with real part zero.
*/
vec2 c_sqrt(vec2 Z){
	vec2 ZPositiveRealPart = vec2(abs(Z.x), Z.y);
	vec2 ComputedRoot = c_sqrtUnsafe(ZPositiveRealPart);
	return (Z.x >= 0.0f) ? ComputedRoot : ComputedRoot.yx;
}
/*! This utility function computes one cubic root of the given complex value. The 
    other roots can be found by multiplication by cubic roots of unity.
    This function has various discontinuities.*/
vec2 c_cbrt(vec2 Z){
	float Argument = atan(Z.y, Z.x);
	float NewArgument = Argument / 3.0f;
	vec2 NormalizedRoot = vec2(cos(NewArgument), sin(NewArgument));
	return NormalizedRoot * pow(dot(Z,Z), 1.0f / 6.0f);
}

mat3x2 SolveCubicBlinn(mat4x2 coeff) {
	vec2 A = coeff[0];
	vec2 B = coeff[1];
	vec2 C = coeff[2];
	vec2 D = coeff[3];
	// Normalize the polynomial
	vec2 InvA = c_recip(A);
	B = c_mul(B,InvA);
	C = c_mul(C,InvA);
	D = c_mul(D,InvA);
	// c_div middle coefficients by three
	B /= 3.0f;
	C /= 3.0f;
	// Compute the Hessian and the discriminant
	vec2 Delta00 = -c_pow2(B) + C;
	vec2 Delta01 = -c_mul(C, B) + D;
	vec2 Delta11 = c_mul(B, D) - c_pow2(C);
	vec2 Discriminant = 4.0f * c_mul(Delta00, Delta11) - c_pow2(Delta01);
	// Compute coefficients of the depressed cubic 
	// (third is zero, fourth is one)
	vec2 DepressedD = -2.0f * c_mul(B, Delta00) + Delta01;
	vec2 DepressedC = Delta00;
	// Take the cubic root of a complex number avoiding cancellation
	vec2 DiscriminantRoot = c_sqrt(-Discriminant);
	DiscriminantRoot = faceforward(DiscriminantRoot, DiscriminantRoot, DepressedD);
	vec2 CubedRoot = DiscriminantRoot - DepressedD;
	vec2 FirstRoot = c_cbrt(0.5f * CubedRoot);
	mat3x2 pCubicRoot={
		FirstRoot,
		c_mul(vec2(-0.5f,-0.5f*sqrt(3.0f)),FirstRoot),
		c_mul(vec2(-0.5f, 0.5f*sqrt(3.0f)),FirstRoot)
	};
	// Also compute the reciprocal cubic roots
	vec2 InvFirstRoot=c_recip(FirstRoot);
	mat3x2 pInvCubicRoot={
		InvFirstRoot,
		c_mul(vec2(-0.5f, 0.5f*sqrt(3.0f)),InvFirstRoot),
		c_mul(vec2(-0.5f,-0.5f*sqrt(3.0f)),InvFirstRoot)
	};
	// Turn them into roots of the depressed cubic and revert the depression 
	// transform
	return mat3x2(
		pCubicRoot[0] - c_mul(DepressedC, pInvCubicRoot[0]) - B,
		pCubicRoot[1] - c_mul(DepressedC, pInvCubicRoot[1]) - B,
		pCubicRoot[2] - c_mul(DepressedC, pInvCubicRoot[2]) - B
	);
}

vec2 depthToFourierBasisFunc(float depth){
  float phase = depth * wrapping_zone_param + wrapping_zone_param;
  return vec2(cos(phase), sin(phase));
}

float psi(vec2 x){
	//Im - Re	  : Re >= 0, Im >= 0
	//2 - Im - Re : Re < 0 , Im >= 0
	//4 - Im + Re : Re < 0 , Im < 0
	//6 + Im + Re : Re >= 0, Im < 0
	mat2x2 result = {
		vec2(x.y - x.x, 2 - x.y - x.x),
		vec2(6 + x.y + x.x, 4 - x.y + x.x)
	};
	return result[(x.y<0)?1:0][(x.x<0)?1:0];
}

float getRootWeightFactor(vec2 x_l, vec2 x_0){
	float p_l = psi(x_l);
	float p_0 = psi(x_0);
	float p_1 = psi(depthToFourierBasisFunc(1.0f));
	float result = (p_l < p_0) ? 1.0f : 0.0f;
	result = (p_l > p_1)
	? (p_l - p_1) / (7.0f - p_1)
	: result;
	return result;
}

vec3 getRootWeightFactor(vec2 reference_point, mat3x2 root_parameters){
	return vec3(
		getRootWeightFactor(reference_point, root_parameters[0]),
		getRootWeightFactor(reference_point, root_parameters[1]),
		getRootWeightFactor(reference_point, root_parameters[2])
	);
}

float computeAbsorbance(mat3x2 b, float depth, float bias, float beta){
	float bias_compl = 1.0f - bias;
	mat4x2 bw = {
		vec2(1.0f, 0.0f) + bias,
		b[0] * bias_compl,
		b[1] * bias_compl,
		b[2] * bias_compl
	};
	// Compute a Cholesky factorization of the Toeplitz matrix
	float D00 = bw[0].x;
	float InvD00 = 1.0f / D00;
	vec2 L10 = (bw[1]) * InvD00;
	float D11 = (bw[0] - D00 * c_mul(L10, c_conj(L10))).x;
	float InvD11 = 1.0f / D11;
	vec2 L20 = (bw[2]) * InvD00;
	vec2 L21 = (bw[1] - D00 * c_mul(L20, c_conj(L10))) * InvD11;
	float D22 = (bw[0] - D00 * c_mul(L20, c_conj(L20)) - D11 * c_mul(L21, c_conj(L21))).x;
	float InvD22 = 1.0f / D22;
	vec2 L30 = (bw[3]) * InvD00;
	vec2 L31 = (bw[2] - D00 * c_mul(L30, c_conj(L10))) * InvD11;
	vec2 L32 = (bw[1] - D00 * c_mul(L30, c_conj(L20)) - D11 * c_mul(L31, c_conj(L21))) * InvD22;
	float D33 = (bw[0] - D00 * c_mul(L30, c_conj(L30)) - D11 * c_mul(L31, c_conj(L31)) - D22 * c_mul(L32, c_conj(L32))).x;
	float InvD33 = 1.0f / D33;
	// Solve a linear system to get the relevant polynomial
	vec2 circle_point = depthToFourierBasisFunc(depth);
	vec2 circle_point_pow2 = c_mul(circle_point, circle_point);
	mat4x2 complexRoots = {
		vec2(1.0f,0.0f),
		circle_point,
		circle_point_pow2,
		c_mul(circle_point, circle_point_pow2)
	};
	complexRoots[1] -= c_mul(L10, complexRoots[0]);
	complexRoots[2] -= c_mul(L20, complexRoots[0]) + c_mul(L21, complexRoots[1]);
	complexRoots[3] -= c_mul(L30, complexRoots[0]) + c_mul(L31, complexRoots[1]) + c_mul(L32, complexRoots[2]);
	complexRoots[0] *= InvD00;
	complexRoots[1] *= InvD11;
	complexRoots[2] *= InvD22;
	complexRoots[3] *= InvD33;
	complexRoots[2] -= c_mul(c_conj(L32), complexRoots[3]);
	complexRoots[1] -= c_mul(c_conj(L21), complexRoots[2]) + c_mul(c_conj(L31), complexRoots[3]);
	complexRoots[0] -= c_mul(c_conj(L10), complexRoots[1]) + c_mul(c_conj(L20), complexRoots[2]) + c_mul(c_conj(L30), complexRoots[3]);
	// Compute roots of the polynomial
	mat3x2 pRoot = SolveCubicBlinn(complexRoots);
	pRoot[0] = normalize(pRoot[0]);
	pRoot[1] = normalize(pRoot[1]);
	pRoot[2] = normalize(pRoot[2]);

	// Figure out how to weight the weights
	vec4 w = vec4(beta, getRootWeightFactor(circle_point, pRoot));

	// Compute the appropriate linear combination of weights
	mat4x2 z = {circle_point, pRoot[0], pRoot[1], pRoot[2]};

	vec4 f1 = w;
	mat3x2 f2 = {
		c_div(f1.y - f1.x, z[1] - z[0]),
		c_div(f1.z - f1.y, z[2] - z[1]),
		c_div(f1.w - f1.z, z[3] - z[2])
	};
	mat2x2 f3 = {
		c_div(f2[1] - f2[0], z[2] - z[0]),
		c_div(f2[2] - f2[1], z[3] - z[1])
	};
	vec2 f4 = c_div(f3[1] - f3[0], z[3] - z[0]);

	mat4x2 polynomial;
	polynomial[0] = f4;
	polynomial[1] = polynomial[0];
	polynomial[0] = f3[0] - c_mul(polynomial[0], z[2]);
	polynomial[2] = polynomial[1];
	polynomial[1] = polynomial[0] - c_mul(polynomial[1], z[1]);
	polynomial[0] = f2[0] - c_mul(polynomial[0], z[1]);
	polynomial[3] = polynomial[2];
	polynomial[2] = polynomial[1] - c_mul(polynomial[2], z[0]);
	polynomial[1] = polynomial[0] - c_mul(polynomial[1], z[0]);
	polynomial[0] = f1[0] - c_mul(polynomial[0], z[0]);

	float weight_sum = 0;
	weight_sum += c_mul(bw[0], polynomial[0]).x;
	weight_sum += c_mul(bw[1], polynomial[1]).x;
	weight_sum += c_mul(bw[2], polynomial[2]).x;
	weight_sum += c_mul(bw[3], polynomial[3]).x;
	return weight_sum;
}

vec3 shade(vec3 normal, vec3 lightDir, vec3 viewDir,
           vec3 Le, vec3 kd, vec3 ks, float shine){
	float cosa = abs(dot(normal, lightDir));
	float cosb = dot(normal, viewDir);
	vec3 halfway = normalize(lightDir + viewDir);
	float cosd = abs(dot(normal, halfway));

	return Le * (kd * cosa + ks * pow(cosd, shine) * (cosa/max(cosb, cosa)));
}

void main(void){
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
	float fragDepth = gl_FragCoord.z;
	float clipDepth = texelFetch(depthSampler, texCoord, 0).r;
	if (fragDepth >= clipDepth) {
		discard;
	}

	float b_0 = texelFetch(totalAbsorbance, texCoord, 0).r;
	if (b_0 == 0.0f) {
		discard;
	}

	mat3x2 b;
	vec4 b_3456 = texelFetch(moment23, texCoord, 0);
	b[0] = texelFetch(moment1, texCoord, 0).rg;
	b[1] = b_3456.xy;
	b[2] = b_3456.zw;
	b /= b_0;
	float T_z_f = exp(-b_0 * computeAbsorbance(b, fragDepth, 0.0000008f, 0.25f));

	transparentTarget = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < nLights; i++){
		vec3 powerDensity = lights[i].Le * ((lights[i].pos.w == 0.0f) ? 1.0f : 1.0f / (4.0f * 3.1415926f));
		vec3 Le = powerDensity/dot(abs(wLightDir[i]), abs(wLightDir[i]));
	
		transparentTarget.rgb += material.ka * lights[i].La
						+ shade(normalize(wNormal), normalize(wLightDir[i]), normalize(wView),
								Le, material.kd, material.ks, material.shine);
	}

	transparentTarget.a = material.alpha * T_z_f;
	transparentTarget.rgb *= transparentTarget.a;
} 
