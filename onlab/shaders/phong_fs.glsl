#version 460
precision highp float;

struct Light {
	vec4 pos;
	vec3 L_a;
	vec3 L_e;
};

struct Material {
	vec3 kd, ks, ka;
	float alpha, shine;
};

uniform mat4		MVP, M, Minv;
uniform vec3		wCamPos;
uniform Light		lights[8];
uniform int			nLights;
uniform Material	material;

in vec4 wLightDir[8];
in vec3 wNormal;
in vec3 wView;
in vec2 texcoord;

out vec4 fragColor;

void main(){
	fragColor = vec4(material.kd, 1);
}