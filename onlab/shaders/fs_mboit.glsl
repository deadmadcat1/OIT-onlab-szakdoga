#version 460
precision highp float;

uniform struct {
  vec3 kd, ks, ka;
  float alpha, shine;
} material;

uniform sampler2D depthSampler;

struct vec5 {
  float v0;
  vec4 v1234;
};

out vec4 fragColor;

void main(void) {
  float clipDepth = texelFetch(depthSampler, ivec2(gl_FragCoord.xy), 0).r;
  float fragDepth = gl_FragCoord.z;
  if (fragDepth >= clipDepth) {
    discard;
  }

  fragColor = vec4(0, 0, 0, 0);
  vec5 moments = mboit(material.alpha, fragDepth);

  //TODO: render to multiple targets(R-RGBA) or figure out how to write depth manually
  fragColor
}

vec5 mboit(float alpha, float depth) {
  float absorbance = -log(1 - alpha);
  float depth2 = depth * depth;
  float depth4 = depth2 * depth2;

  vec5 output;
  output.v0 = absorbance;
  output.v1234 = vec4(depth, depth2, depth2 * depth, depth4) * absorbance;
  return output;
}
