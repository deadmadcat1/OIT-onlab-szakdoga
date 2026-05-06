#version 460
precision highp float;

uniform int nLights;
uniform struct {
  vec4 pos;
  vec3 La;
  vec3 Le;
} lights[8];

uniform struct {
	mat4 V;
	mat4 P;
	vec3 wPos;
	float near;
	float far;
} camera;

uniform struct {
  vec3 kd, ks, ka;
  float alpha, shine;
} material;

uniform sampler2D depthSampler;
uniform sampler2D coeff1;
uniform sampler2D coeff2;
uniform sampler2D coeff3;
uniform sampler2D coeff4;
uniform sampler2D coeff5;

uniform uvec2 viewportSize;

in vec3 wLightDir[8];
in vec4 wPos;
in vec3 wNormal;
in vec3 wView;

layout(location = 0) out vec4 fragColor;

float linearize(float z) {
  float z_ndc = z * 2.0 - 1.0f;
  return (2.0f * camera.near * camera.far) / (camera.far + camera.near - z_ndc * (camera.far - camera.near));
}

float scalingFunction(float z) {
  return (z < 0.0f || z > 1.0f) ? 0.0f : 1.0f;
}

float haar(uint scale, uint translation, float t) {
  uint expo = 1 << scale;
  float disc = expo * t - translation;
  float norm = exp2(-(scale / 2.0f));
  return (disc < 0.0f || disc > 1.0f) ? 0.0f : norm * ((disc < 0.5f) ? disc : 1.0f - disc);
}

vec3 shade(vec3 normal, vec3 lightDir, vec3 viewDir,
  vec3 Le, vec3 kd, vec3 ks, float shine) {
  float cosa = abs(dot(normal, lightDir));
  float cosb = dot(normal, viewDir);
  vec3 halfway = normalize(lightDir + viewDir);
  float cosd = abs(dot(normal, halfway));

  return Le * (kd * cosa + ks * pow(cosd, shine) * (cosa / max(cosb, cosa)));
}

void main(void) {
  vec2 fragCoord = gl_FragCoord.xy / viewportSize;
  float clipDepth = texture(depthSampler, fragCoord).r;
  if (gl_FragCoord.z >= clipDepth) {
    discard;
  }

  vec3 coeff1Raw = texture(coeff1, fragCoord).rgb;
  vec3 coeff2Raw = texture(coeff2, fragCoord).rgb;
  vec3 coeff3Raw = texture(coeff3, fragCoord).rgb;
  vec3 coeff4Raw = texture(coeff4, fragCoord).rgb;
  vec3 coeff5Raw = texture(coeff5, fragCoord).rgb;

  float scalingCoeff = coeff1Raw.r;
  vec2 scale1Coeff = coeff1Raw.gb;
  vec4 scale2Coeff = vec4(coeff2Raw.rgb, coeff3Raw.r);
  vec4 scale3Coeff1 = vec4(coeff3Raw.gb, coeff4Raw.rg);
	vec4 scale3Coeff2 = vec4(coeff4Raw.b, coeff5Raw);

  float linearZ = linearize(gl_FragCoord.z);
  float highPassFactor =
    dot(scale1Coeff, vec2(
        haar(1, 0, linearZ),
        haar(1, 1, linearZ)
      )) +
    dot(scale2Coeff, vec4(
      haar(2, 0, linearZ),
      haar(2, 1, linearZ),
      haar(2, 2, linearZ),
      haar(2, 3, linearZ)
    )) +
    dot(scale3Coeff1, vec4(
      haar(3, 0, linearZ),
      haar(3, 1, linearZ),
      haar(3, 2, linearZ),
      haar(3, 3, linearZ)
    )) +
    dot(scale3Coeff2, vec4(
      haar(3, 4, linearZ),
      haar(3, 5, linearZ),
      haar(3, 6, linearZ),
      haar(3, 7, linearZ)
    ));
  float absorbance = scalingCoeff * scalingFunction(linearZ) - highPassFactor;

  fragColor = vec4(0, 0, 0, 1);
  for (int i = 0; i < nLights; i++) {
    vec3 powerDensity = lights[i].Le * ((lights[i].pos.w == 0.0f) ? 1.0f : 1.0f / (4.0f * 3.1415926f));
    vec3 Le = powerDensity / dot(abs(wLightDir[i]), abs(wLightDir[i]));

    fragColor.rgb += material.ka * lights[i].La
        + shade(normalize(wNormal), normalize(wLightDir[i]), normalize(wView),
          Le, material.kd, material.ks, material.shine);
  }
  fragColor.a = material.alpha * exp(-absorbance);
  fragColor.rgb *= fragColor.a;
}
