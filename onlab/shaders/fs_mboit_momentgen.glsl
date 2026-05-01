#version 460
precision highp float;

uniform struct {
  vec3 kd, ks, ka;
  float alpha, shine;
} material;

uniform float wrapping_zone_param;

uniform sampler2D depthSampler;

layout(location = 0) out float totalAbsorbance;
layout(location = 1) out vec2 moment1;
layout(location = 2) out vec4 moment23;

vec2 c_mul(vec2 lhs, vec2 rhs){
  return vec2(lhs.x * rhs.x - lhs.y * rhs.y, lhs.x * rhs.y + lhs.y * rhs.x);
}

vec2 depthToFourierBasisFunc(float depth){
  float phase = depth * wrapping_zone_param + wrapping_zone_param;
  return vec2(cos(phase), sin(phase));
}

void main(void) {
  float clipDepth = texelFetch(depthSampler, ivec2(gl_FragCoord.xy), 0).r;
  float fragDepth = gl_FragCoord.z;
  if (fragDepth >= clipDepth) {
    discard;
  }

  float absorbance = -log(1.0f - material.alpha);

  vec2 circle_point = depthToFourierBasisFunc(fragDepth);
  vec2 circle_point_pow2 = c_mul(circle_point, circle_point);

	totalAbsorbance = absorbance; //additively blended
  moment1 = circle_point * absorbance;
  moment23 = vec4(circle_point_pow2, c_mul(circle_point, circle_point_pow2)) * absorbance;
}
