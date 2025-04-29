#pragma once
#include <glm/glm.hpp>
#include <string>

class Light {
	int idx;
	glm::vec4 position = glm::vec4(0.0f);
	glm::vec3 ambientLuminosity = glm::vec3(0.0f);
	glm::vec3 emissiveLuminosity = glm::vec3(0.0f);
public:
	explicit Light(int lightIdx) : idx(lightIdx) {}

	void bindUniforms(const std::shared_ptr<GPUProgram>& program) {
		std::string baseHandle("lights[" + std::to_string(idx) + "]");
		program->activate();
		program->setUniform(baseHandle + ".pos", &position);
		program->setUniform(baseHandle + ".La", &ambientLuminosity);
		program->setUniform(baseHandle + ".Le", &emissiveLuminosity);
	}
	void translate(glm::vec3 deltaPos) {
		position += glm::vec4(deltaPos, 0.0f);
	}
	void setAmbientColor(glm::vec3 La) {
		ambientLuminosity = La;
	}
	void setEmissiveColor(glm::vec3 Le) {
		emissiveLuminosity = Le;
	}
	void setPointLight() {
		position.w = 1.0f;
	}
	void setDirectionalLight() {
		position.w = 0.0f;
	}
};