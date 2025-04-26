#pragma once
#include "GPUProgram.h"
#include <glm/vec3.hpp>

class Material {
public:
	glm::vec3 kd = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ka = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ks = glm::vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	float shine = 1.0f;

	void bindUniforms(std::shared_ptr<GPUProgram> gpuProgram) {
		gpuProgram->activate();
		gpuProgram->setUniform("material.kd", &kd);
		gpuProgram->setUniform("material.ka", &ka);
		gpuProgram->setUniform("material.ks", &ks);
		gpuProgram->setUniform("material.alpha", alpha);
		gpuProgram->setUniform("material.shine", shine);
	}
};