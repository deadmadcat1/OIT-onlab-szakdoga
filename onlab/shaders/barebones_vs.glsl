#version 460 core
precision highp float;

struct Light {
	vec4 pos;
	vec3 L_a;
	vec3 L_e;
};

layout(location = 0) in vec3  vtxPos;
layout(location = 1) in vec3  vtxNormal;
layout(location = 2) in vec2  vtxUV;

uniform mat4  MVP, M, Minv;
uniform vec3  wCamPos;
uniform Light lights[8];
uniform int   nLights;

out vec3 wLightDir[8];
out vec3 wNormal;
out vec3 wView;
out vec2 texcoord;

void main(){
	gl_Position = vec4(vtxPos, 1) * MVP;
	vec4 wPos = vec4(vtxPos, 1) * M;
	for(int i = 0; i < nLights; i++) {
		wLightDir[i] = lights[i].pos.xyz * wPos.w - wPos.xyz * lights[i].pos.w;
	}
	wView  = wCamPos * wPos.w - wPos.xyz;
	wNormal = (Minv * vec4(vtxNormal, 0)).xyz;
	texcoord = vtxUV;
}