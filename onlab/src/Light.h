#pragma once
#include <glm/glm.hpp>

class Light {
public:
	glm::vec4 position;
	glm::vec3 powerDensity;
	void bindUniforms(GPUProgram* program) {
		program->setUniform(&position, "light");
		program->setUniform(&powerDensity, "Material.ka");
	}
};