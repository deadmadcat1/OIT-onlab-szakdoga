#version 460
precision highp float;

uniform sampler2D colorSampler1;

in vec2 texCoord;

out vec4 fragColor;

void main(void){
	fragColor = texture(colorSampler1, texCoord);
}