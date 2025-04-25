#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include "GPUProgram.h"

class Material {
public:
	//std::unordered_map <std::string, std::shared_ptr<void>> uniforms;
	glm::vec3 kd = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ka = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ks = glm::vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	float shine = 1.0f;

	void bindUniforms(std::shared_ptr<GPUProgram> gpuProgram) {
		gpuProgram->activate();
		//for (const auto& uniform : uniforms) {
		//	gpuProgram->setUniform(std::dynamic_pointer_cast<???>(uniform.second), "Material." + uniform.first)
		//}
		gpuProgram->setUniform(&kd, "Material.kd");
		gpuProgram->setUniform(&ka, "Material.ka");
		gpuProgram->setUniform(&ks, "Material.ks");
		gpuProgram->setUniform(&ks, "Material.alpha");
		gpuProgram->setUniform(shine, "Material.shine");
	}
};