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
<<<<<<< HEAD
	mat4 VP;
	vec3 wPos;
=======
	mat4 V;
	mat4 P;
	vec3 wPos;
	float near;
	float far;
>>>>>>> 27bc4f4 (new repo init)
} camera;

uniform struct {
	mat4  M, Minv;
} object;

out vec3 wLightDir[8];
out vec4 wPos;
out vec3 wNormal;
out vec3 wView;

void main(){
<<<<<<< HEAD
	mat4 MVP = camera.VP * object.M;
=======
	mat4 MVP = camera.P * camera.V * object.M;
>>>>>>> 27bc4f4 (new repo init)
	wPos = object.M * vec4(vtxPos, 1.0f);
	wNormal = (vec4(vtxNormal, 0.0f) * object.Minv).xyz;
	wView  = camera.wPos - wPos.xyz;
	for(int i = 0; i < nLights; i++) {
		wLightDir[i] = lights[i].pos.xyz - wPos.xyz * lights[i].pos.w;
	}
	gl_Position = MVP * vec4(vtxPos, 1.0f);
<<<<<<< HEAD
}
=======
}
>>>>>>> 27bc4f4 (new repo init)
