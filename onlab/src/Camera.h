#pragma once
#include <glm/glm.hpp>
#include "GPUProgram.h"

class Camera {
public:
	glm::mat4 View, Proj;
	glm::vec3 pos, lookAt, viewUp;
	float fov, asp, fp, bp;

	void update();

	void bindUniforms(GPUProgram* program);
};