#include "Camera.h"
#include <glm/mat4x4.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Camera::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 View = glm::translate(glm::mat4(1.0f), -position) * glm::toMat4(orientation);

	glm::mat4 Proj = glm::perspective(vfov, aratio, fp, bp);

	program->activate();
	program->setUniform("camera.wPos", &position);
	glm::mat4 VP = View * Proj;
	program->setUniform("camera.VP", &VP);
}

void Camera::setParams(float _vfov = 73.0f, float _aratio = (16.0f / 9.0f), float _fp = 0.1f, float _bp = 100.0f) {
	vfov = _vfov;
	aratio = _aratio;
	fp = _fp;
	bp = _bp;
}

void Camera::setViewUp(glm::vec3 direction) {
	viewUp = direction;
}

void Camera::translate(glm::vec3 deltaPos) {
	position += deltaPos;
}

void Camera::rotate(glm::vec3 amountPerAxis) {
	glm::quat rotQuat(amountPerAxis);
	orientation *= rotQuat;
}

void Camera::lookAt(glm::vec3 point) {
	orientation = glm::quatLookAt(position-point, viewUp);
}

void Camera::orbit(glm::vec3 point, glm::vec3 amountPerAxis) {
	glm::quat rotQuat(amountPerAxis);
	position = point + (rotQuat * (position - point));
}