#pragma once
#include <glm/glm.hpp>
#include <string>

class Light {
public:
	glm::vec4 position;
	glm::vec3 ambientLuminosity;
	glm::vec3 emissiveLuminosity;
	int idx;

	explicit Light(int lightIdx) : idx(lightIdx) {}

	void bindUniforms(GPUProgram* program) {
		std::string baseHandle("lights[" + std::to_string(idx) + "]");
		program->activate();
		program->setUniform(&position, (baseHandle + ".pos").c_str());
		program->setUniform(&ambientLuminosity, (baseHandle + ".L_a").c_str());
		program->setUniform(&emissiveLuminosity, (baseHandle + ".L_e").c_str());
	}
};