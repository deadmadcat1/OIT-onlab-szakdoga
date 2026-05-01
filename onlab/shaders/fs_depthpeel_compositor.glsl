#version 460
precision highp float;

uniform sampler2D peel1;
uniform sampler2D peel2;
uniform sampler2D peel3;
uniform sampler2D peel4;

in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

vec4 blend(vec4 layerA, vec4 layerB){
	float alpha = layerA.a + layerB.a * (1 - layerA.a);
	vec3 color = layerA.rgb + layerB.rgb * (1 - layerA.a);
	
	return vec4(color, alpha);
}

void main(void){
	fragColor = vec4(0,0,0,1);
	vec4 layer1 = texture(peel1, texCoord);
	vec4 layer2 = texture(peel2, texCoord);
	vec4 layer3 = texture(peel3, texCoord);
	vec4 layer4 = texture(peel4, texCoord);
	fragColor = blend(layer1, blend(layer2, blend(layer3, layer4)));
}
