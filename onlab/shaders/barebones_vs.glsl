#version 460 core
precision highp float;

layout(location = 0) in vec3  vtxPos;
layout(location = 1) in vec3  vtxNormal;
layout(location = 2) in vec2  vtxUV;

uniform int   nLights;
uniform struct {
	vec4 pos;
	vec3 L_a;
	vec3 L_e;
} lights[8];

uniform struct {
	mat4 VP;
	vec3 wPos;
} camera;

uniform mat4  M, Minv;

out vec3 wLightDir[8];
out vec3 wNormal;
out vec3 wView;
out vec2 texcoord;

void main(){
	mat4 MVP = M * camera.VP;
	gl_Position = vec4(vtxPos, 1) * MVP;
	vec4 wPos = vec4(vtxPos, 1) * M;
	for(int i = 0; i < nLights; i++) {
		wLightDir[i] = lights[i].pos.xyz * wPos.w - wPos.xyz * lights[i].pos.w;
	}
	wView  = camera.wPos * wPos.w - wPos.xyz;
	wNormal = (Minv * vec4(vtxNormal, 0)).xyz;
	texcoord = vtxUV;
}