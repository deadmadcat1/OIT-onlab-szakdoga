#version 460
precision highp float;

uniform sampler2D mainPassColor;

in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

void main(void){
	fragColor = texture(mainPassColor, texCoord);
}