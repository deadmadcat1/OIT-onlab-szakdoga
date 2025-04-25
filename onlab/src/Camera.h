#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "GPUProgram.h"

class Camera {
public:
	glm::mat4 View;
	glm::mat4 Proj;
	glm::vec3 pos = glm::vec3(12.0f);
	glm::vec3 lookAt = glm::vec3(0.0f);
	glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float fov = 73.0f;
	float asp = 4/3;
	float fp = 0.1f;
	float bp = 100.0f;

	void update();

	void bindUniforms(GPUProgram* program);
};