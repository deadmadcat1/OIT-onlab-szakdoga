#pragma once
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL	//only for glm::toMat4 to not throw compile error, it is just a wrapper for glm::mat4_cast(glm::quat)
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "GPUProgram.h"

class Camera {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lookAtP = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	float vfov = 73.0f;
	float aratio = 16.0f/9.0f;
	float fp = 0.1f;
	float bp = 100.0f;
public:
	void bindUniforms(std::shared_ptr<GPUProgram> program);
	void setParams(float vfov = 73.0f, float aratio = (16.0f / 9.0f), float nearp = 0.1f, float farp = 100.0f);
	void setViewUp(glm::vec3 direction);
	void translate(glm::vec3 deltaPos);
	void rotate(glm::vec3 amountPerAxis);
	void lookAt(glm::vec3 point);
	void orbit(glm::vec3 point, glm::vec3 amountPerAxis);
};