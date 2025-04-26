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
		program->setUniform(baseHandle + ".pos", &position);
		program->setUniform(baseHandle + ".L_a", &ambientLuminosity);
		program->setUniform(baseHandle + ".L_e", &emissiveLuminosity);
	}
	void translate(glm::vec3 deltaPos) {
		position += glm::vec4(deltaPos, 0.0f);
	}
	void changeAmbientColor(glm::vec3 deltaLa) {
		ambientLuminosity += deltaLa;
	}
	void changeEmissiveColor(glm::vec3 deltaLe) {
		emissiveLuminosity += deltaLe;
	}
	void setPointLight() {
		position.w = 1.0f;
	}
	void setDirectionalLight() {
		position.w = 0.0f;
	}
};