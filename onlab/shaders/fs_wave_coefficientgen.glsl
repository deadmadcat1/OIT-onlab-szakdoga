#version 460
precision highp float;

uniform struct {
	vec3 kd, ks, ka;
	float alpha, shine;
} material;

uniform struct {
	mat4 V;
	mat4 P;
	vec3 wPos;
	float near;
	float far;
} camera;

layout(location = 0) out vec3 coeff1;
layout(location = 1) out vec3 coeff2;
layout(location = 2) out vec3 coeff3;
layout(location = 3) out vec3 coeff4;
layout(location = 4) out vec3 coeff5;

float linearize(float z){
	float z_ndc = z * 2.0 - 1.0f;
	return (2.0f * camera.near * camera.far) / (camera.far + camera.near - z_ndc * (camera.far - camera.near));
}

float haar(uint scale, uint translation, float t){
	uint expo = 1 << scale;
	float disc = expo * t - translation;
	float norm = exp2(-(scale / 2.0f));
	return (disc < 0.0f || disc > 1.0f) ? 0.0f : norm * ((disc < 0.5f) ? disc : 1.0f - disc);
}

void main(void) {
	float linearZ = linearize(gl_FragCoord.z);
	coeff1.r = 1.0f - linearZ;
	coeff1.gb = vec2(haar(1, 0, linearZ), haar(1,1,linearZ));
	coeff2.rgb = vec3(
			haar(2, 0, linearZ),
			haar(2, 1, linearZ),
			haar(2, 2, linearZ)
		);
	coeff3.r = haar(2, 3, linearZ);
	coeff3.gb = vec2(
			haar(3, 0, linearZ),
			haar(3, 1, linearZ)
		);
	coeff4.rgb = vec3(
			haar(3, 2, linearZ),
			haar(3, 3, linearZ),
			haar(3, 4, linearZ)
		);
	coeff5.rgb = vec3(
			haar(3, 5, linearZ),
			haar(3, 6, linearZ),
			haar(3, 7, linearZ)
		);
	coeff1 *= material.alpha;
	coeff2 *= material.alpha;
	coeff3 *= material.alpha;
	coeff4 *= material.alpha;
	coeff5 *= material.alpha;
}
