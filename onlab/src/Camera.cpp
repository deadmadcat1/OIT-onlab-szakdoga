#include "Camera.h"
#include <glm/mat4x4.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Camera::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 View = glm::lookAt(position, lookAtP, viewUp);
	
	glm::mat4 Proj = glm::perspective(glm::radians(_vfov), _aratio, _fp, _bp);

	program->activate();
	program->setUniform("camera.wPos", &position);
	glm::mat4 VP = Proj * View;
	program->setUniform("camera.VP", &VP);
}

void Camera::setParams(float vfov, float aratio, float fp, float bp) {
	_vfov = vfov;
	_aratio = aratio;
	_fp = fp;
	_bp = bp;
}

void Camera::setViewUp(glm::vec3 direction) {
	viewUp = direction;
}

void Camera::translate(glm::vec3 amountPerAxis) {
	position += amountPerAxis;
}

void Camera::rotate(glm::vec3 degreesPerAxis) {
	glm::quat rotQuat(glm::radians(degreesPerAxis));
	lookAtP = glm::rotate(rotQuat, lookAtP);
	viewUp = glm::rotate(rotQuat, viewUp);
}

void Camera::lookAt(glm::vec3 point) {
	lookAtP = point;
}

void Camera::orbit(glm::vec3 point, glm::vec3 degreesPerAxis) {
	glm::quat rotQuat(glm::radians(degreesPerAxis));
	rotQuat = glm::normalize(rotQuat);
	position = point + (rotQuat * (position - point));
}