#include "Camera.h"
#include <glm/mat4x4.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Camera::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 View = glm::lookAt(position, lookat, viewup);
	
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
	viewup = glm::normalize(direction);
}

void Camera::lookAt(glm::vec3 point) {
	lookat = glm::normalize(point - position);
}

void Camera::translate(glm::vec3 amountPerAxis) {
	position += amountPerAxis;
}

void Camera::rotate(glm::vec3 degreesPerAxis) {
	glm::vec3 rad = glm::radians(degreesPerAxis);
	glm::mat4 rotMat = glm::rotate(glm::mat4(), rad.x, glm::vec3(1,0,0));
	rotMat = glm::rotate(rotMat, rad.y, glm::vec3(0, 1, 0));
	lookAt(glm::vec3(rotMat * glm::vec4(glm::normalize(lookat), 1)));
	setViewUp(glm::vec3(rotMat * glm::vec4(viewup, 1)));
}

void Camera::rotateNDC(float pitch, float yaw) {
	float hfov = _aratio * _vfov;
	rotate(glm::vec3(0.5f * _vfov * pitch, 0/*0.5 * hfov * yaw*/, 0));
}

void Camera::orbit(glm::vec3 point, glm::vec3 degreesPerAxis) {
	glm::quat rotQuat(glm::radians(degreesPerAxis));
	rotQuat = glm::normalize(rotQuat);
	position = point + (rotQuat * (position - point));
}