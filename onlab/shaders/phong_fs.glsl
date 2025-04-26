#version 460
precision highp float;

uniform int	nLights;
uniform struct {
	vec4 pos;
	vec3 L_a;
	vec3 L_e;
} lights[8];

uniform struct {
	vec3 kd, ks, ka;
	float alpha, shine;
} material;

uniform struct {
	mat4 VP;
	vec3 wPos;
} camera;

uniform mat4 M, Minv;

in vec4 wLightDir[8];
in vec3 wNormal;
in vec3 wView;
in vec2 texcoord;

out vec4 fragColor;

void main(){
	fragColor = vec4(material.kd, 1);
}