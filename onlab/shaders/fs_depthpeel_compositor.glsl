#version 460
precision highp float;

uniform sampler2D colorSampler1;
uniform sampler2D colorSampler2;
uniform sampler2D colorSampler3;
uniform sampler2D colorSampler4;

in vec2 texCoord;

out vec4 fragColor;

vec4 blend(vec4 layerA, vec4 layerB){
	float alpha = layerA.a + layerB.a * (1 - layerA.a);
	vec3 color = layerA.rgb * layerA.a  + layerB.a * layerB.rgb * (1 - layerA.a);
	color = color / alpha;
	
	return vec4(color, alpha);
}

void main(void){
	fragColor = vec4(0,0,0,1);
	vec4 layer1 = texture(colorSampler1, texCoord);
	vec4 layer2 = texture(colorSampler2, texCoord);
	vec4 layer3 = texture(colorSampler3, texCoord);
	vec4 layer4 = texture(colorSampler4, texCoord);
	fragColor = blend(layer1, blend(layer2, blend(layer3, layer4)));
	//fragColor = blend(blend(blend(layer1, layer2), layer3), layer4);;
}