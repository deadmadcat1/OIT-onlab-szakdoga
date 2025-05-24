#version 460
precision highp float;

uniform int	nLights;
uniform struct {
	vec4 pos;
	vec3 La;
	vec3 Le;
} lights[8];

uniform struct {
	vec3 kd, ks, ka;
	float alpha, shine;
} material;

uniform sampler2D depthSampler;

in vec3 wLightDir[8];
in vec4 wPos;
in vec3 wNormal;
in vec3 wView;

out vec4 fragColor;

vec3 shade(vec3 normal, vec3 lightDir, vec3 viewDir,
           vec3 Le, vec3 kd, vec3 ks, float shine){
	float cosa = max(dot(normal, lightDir), 0.0f);
	float cosb = max(dot(normal, viewDir), 0.0f);
	vec3 halfway = normalize(lightDir + viewDir);
	float cosd = max(dot(normal, halfway), 0.0f);
	return Le * (kd * cosa + ks * pow(cosd, shine) * (cosa/max(cosb, cosa)));
}

void main(void){
#ifdef DEPTH_PEEL_ENABLED
	float clipDepth = texelFetch(depthSampler, ivec2(gl_FragCoord.xy), 0).r;
	if (gl_FragCoord.z < clipDepth) {
		discard;
	}
#endif
	fragColor = vec4(0,0,0,1);
	for (int i = 0; i < nLights; i++){
		float oneoverfourpi = 1.0f / (4.0f * 3.1415926f);
		vec3 powerDensity = lights[i].Le * ((lights[i].pos.w == 0.0f) ? 1.0f : oneoverfourpi);
		vec3 Le = powerDensity/dot(abs(wLightDir[i]), abs(wLightDir[i]));
	
		fragColor.rgb += material.ka * lights[i].La
						+ shade(normalize(wNormal), normalize(wLightDir[i]), normalize(wView),
								Le, material.kd, material.ks, material.shine);
		fragColor.a = material.alpha;
	}
}