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

in vec3 wLightDir[8];
in vec4 wPos;
in vec3 wNormal;
in vec3 wView;

out vec4 fragColor;

void main(void){
	fragColor = vec4(material.kd, material.alpha);
}