#version 460 core
precision highp float;

layout(location = 0) in vec3  vtxPos;
layout(location = 1) in vec3  vtxNormal;
layout(location = 2) in vec2  vtxUV;

uniform int nLights;
uniform struct {
	vec4 pos;
	vec3 La;
	vec3 Le;
} lights[8];

uniform struct {
	mat4 VP;
	vec3 wPos;
} camera;

uniform struct {
	mat4  M, Minv;
} object;

out vec3 wLightDir[8];
out vec4 wPos;
out vec3 wNormal;
out vec3 wView;

void main(){
	mat4 MVP = camera.VP * object.M;
	wPos = object.M * vec4(vtxPos, 1);
	wNormal = (vec4(vtxNormal, 0) * object.Minv).xyz;
	wView  = camera.wPos - wPos.xyz;
	for(int i = 0; i < nLights; i++) {
		wLightDir[i] = lights[i].pos.xyz - wPos.xyz * lights[i].pos.w;
	}
	gl_Position = MVP * vec4(vtxPos, 1);
}