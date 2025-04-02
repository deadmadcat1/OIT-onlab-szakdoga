#include "Camera.h"
#include "GPUProgram.h"
#include <glm/ext.hpp>

void Camera::update() {
	glm::vec3 w = glm::normalize(pos - lookAt);
	glm::vec3 u = glm::normalize(glm::cross(viewUp, w));
	glm::vec3 v = glm::cross(w, u);

	View = glm::translate(View, -pos) * glm::mat4(u.x, u.y, u.z, 0,
		v.x, v.y, v.z, 0,
		w.x, w.y, w.z, 0,
		0, 0, 0, 1);

	Proj = glm::mat4(1 / (tan(fov * 0.5) * asp), 0, 0, 0,
		0, 1 / tan(fov * 0.5), 0, 0,
		0, 0, -(fp + bp) / (bp - fp), -2 * fp * bp / (bp - fp),
		0, 0, -1, 0);
}

void Camera::bindUniforms(GPUProgram* program) {
	program->activate();
	program->setUniform(&pos, "wCamPos");
}