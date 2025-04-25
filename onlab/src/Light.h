#pragma once
#include <glm/glm.hpp>
#include <string>

class Light {
	int idx;
	glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 ambientLuminosity = glm::vec3(0.0f);
	glm::vec3 emissiveLuminosity = glm::vec3(0.5f);
	
public:
	explicit Light(int lightIdx) : idx(lightIdx) {}

	void bindUniforms(const std::shared_ptr<GPUProgram>& program) {
		std::string baseHandle("lights[" + std::to_string(idx) + "]");
		program->activate();
		program->setUniform(&position, baseHandle + ".pos");
		program->setUniform(&ambientLuminosity, baseHandle + ".L_a");
		program->setUniform(&emissiveLuminosity, baseHandle + ".L_e");
	}
	void translate(glm::vec3 by) {
		position += glm::vec4(by, 0.0f);
	}
	void changeAmbientColor(glm::vec3 by) {
		ambientLuminosity += by;
	}
	void changeEmissiveColor(glm::vec3 by) {
		emissiveLuminosity += by;
	}
	void setPointLight() {
		position.w = 1.0f;
	}
	void setDirectionalLight() {
		position.w = 0.0f;
	}
};