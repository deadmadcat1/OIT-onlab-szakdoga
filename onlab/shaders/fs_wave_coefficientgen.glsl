#version 460
precision highp float;

uniform struct {
	vec3 kd, ks, ka;
	float alpha, shine;
} material;

uniform struct {
	mat4 V;
	mat4 P;
	vec3 wPos;
	float near;
	float far;
} camera;

uniform sampler2D depthTransparent;

uniform uvec2 viewportSize;

layout(location = 0) out uint coeff1;
layout(location = 1) out uint coeff2;
layout(location = 2) out uint coeff3;
layout(location = 3) out uint coeff4;
layout(location = 4) out uint coeff5;

uint pack_rgb9e5(vec3 color) {
	//	https://registry.khronos.org/OpenGL/extensions/EXT/EXT_texture_shared_exponent.txt

  //  Components red, green, and blue are first clamped (in the process,
  //  mapping NaN to zero) so:
	vec3 clamped = clamp(color * 32768, 0, 32768/* sharedexp_max */);

  //  For the RGB9_E5_EXT format, N=9, Emax=31, and B=15.

  //  The largest clamped component, max_c, is determined:
  float max_c = max(clamped.r, max(clamped.g, clamped.b));

  //  A preliminary shared exponent is computed:
  int exp_shared = max(-16, int(floor(log2(max_c)))) + 16;

  //  A refined shared exponent is then computed as:
	float denom = exp2(exp_shared - 24);
  uint max_s = int(floor(max_c / denom + 0.5f));
	exp_shared += (max_s == 512) ? 1 : 0;

  //  These integers values in the range 0 to 2^N-1 are then computed:
	uvec3 packed_val = uvec3(floor(clamped / denom + 0.5f));

	return (packed_val.r << 23) | (packed_val.g << 14) | (packed_val.b << 5) | (exp_shared & 0x1F);
}

float linearize(float z, float maxz){
	float z_ndc = z * 2.0f - 1.0f;
	float maxz_ndc = maxz * 2.0f - 1.0f;
	float linear = camera.P[3][2] / (camera.P[2][2] + z_ndc);
	float linear_max = camera.P[3][2] / (camera.P[2][2] + maxz_ndc);
	return (linear - camera.near) / (linear_max - camera.near);
}

float haar(uint scale, uint translation, float t){
	float expo = exp2(scale);
	float disc = expo * t - translation;
	float norm = exp2(-(scale / 2.0f));
	return (disc < 0.0f || disc > 1.0f) ? 0.0f : norm * ((disc < 0.5f) ? disc : 1.0f - disc);
}

void main(void) {
  vec2 fragCoord = gl_FragCoord.xy / viewportSize;
	float maxZ = texture(depthTransparent, fragCoord).r;
	float linearZ = linearize(gl_FragCoord.z, maxZ);
	vec3 coefficients[5];
	uint packed_val[5];

	coefficients[0] = vec3(1.0f - linearZ, haar(1,0,linearZ), haar(1,1,linearZ));
	coefficients[1] = vec3(haar(2,0,linearZ), haar(2,1,linearZ), haar(2,2,linearZ));
	coefficients[2] = vec3(haar(2,3,linearZ), haar(3,0,linearZ), haar(3,1,linearZ));
	coefficients[3] = vec3(haar(3,2,linearZ), haar(3,3,linearZ), haar(3,4,linearZ));
	coefficients[4] = vec3(haar(3,5,linearZ), haar(3,6,linearZ), haar(3,7,linearZ));

	for (int i = 0; i < 5; i++) {
		packed_val[i] = pack_rgb9e5(coefficients[i] * material.alpha);
	}

	coeff1 = packed_val[0];
	coeff2 = packed_val[1];
	coeff3 = packed_val[2];
	coeff4 = packed_val[3];
	coeff5 = packed_val[4];
}
