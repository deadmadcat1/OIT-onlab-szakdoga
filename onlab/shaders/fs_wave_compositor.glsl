#version 460
precision highp float;

uniform sampler2D opaqueTarget;
uniform sampler2D transparentTarget;
uniform sampler2D coeff1;

in vec2 texCoord;

layout(location = 0) out vec3 fragColor;

void main(void) {
	fragColor = vec3(0.0f, 0.0f, 0.0f);
	vec4 opaque = texture(opaqueTarget, texCoord);
	vec4 premul_transparent = texture(transparentTarget, texCoord);
	float T_total = exp(-texture(coeff1, texCoord).r);
	//float renormalize_factor = (1.0f - T_total);
	//if (premul_transparent.a > 0) {
	//	renormalize_factor /= premul_transparent.a;
	//}
	//fragColor = T_total * opaque.rgb + premul_transparent.rgb * renormalize_factor;
	fragColor = T_total * opaque.rgb + premul_transparent.rgb;
}
