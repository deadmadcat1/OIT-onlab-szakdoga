#pragma once
#include <glm/glm.hpp>
#include "GPUProgram.h"

class Material {
public:
	std::shared_ptr<GPUProgram> gpuProgram;
	glm::vec3 kd, ka, ks;
	float shine;

	Material(std::shared_ptr<GPUProgram> _gpuprogram) : gpuProgram(_gpuprogram), kd(1, 1, 1), ks(1, 1, 1), ka(1, 1, 1), shine(1.0f) {}

	void bindUniforms() {
		gpuProgram->activate();
		gpuProgram->setUniform(&kd, "Material.kd");
		gpuProgram->setUniform(&ka, "Material.ka");
		gpuProgram->setUniform(&ks, "Material.ks");
		gpuProgram->setUniform(shine, "Material.shine");
	}
};