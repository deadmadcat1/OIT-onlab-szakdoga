#pragma once
#include <glm/glm.hpp>
#include "GPUProgram.h"

class Material {
public:
	GPUProgram* gpuProgram;
	glm::vec3 kd, ka, ks;
	float shine;

	Material(GPUProgram* _gpuprogram) : gpuProgram(_gpuprogram) {}

	void bindUniforms() {
		gpuProgram->setUniform(&kd, "Material.kd");
		gpuProgram->setUniform(&ka, "Material.ka");
		gpuProgram->setUniform(&ks, "Material.ks");
		gpuProgram->setUniform(shine, "Material.shine");
	}
};