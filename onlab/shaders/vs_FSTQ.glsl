#version 460 core
precision highp float;

layout(location = 0) in vec3  vtxPos;
layout(location = 2) in vec2  vtxUV;

out vec2 texCoord;

void main(){
	gl_Position = vec4(vtxPos.x, vtxPos.y, 0.0f, 1.0f);
	texCoord = vtxUV;
}