#pragma once
#include <glm/glm.hpp>
#include "GPUProgram.h"

class Material {
public:
	std::shared_ptr<GPUProgram> gpuProgram;
	glm::vec3 kd = glm::vec3(1);
	glm::vec3 ka = glm::vec3(1);
	glm::vec3 ks = glm::vec3(1);
	float shine = 1.0f;

	explicit Material(std::shared_ptr<GPUProgram> _gpuprogram) : gpuProgram(_gpuprogram) {}

	void bindUniforms() {
		gpuProgram->activate();
		gpuProgram->setUniform(&kd, "Material.kd");
		gpuProgram->setUniform(&ka, "Material.ka");
		gpuProgram->setUniform(&ks, "Material.ks");
		gpuProgram->setUniform(shine, "Material.shine");
	}
};