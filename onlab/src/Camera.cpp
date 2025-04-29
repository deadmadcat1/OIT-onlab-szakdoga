#include "Camera.h"
#include <glm/mat4x4.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Camera::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 View = glm::translate(glm::identity<glm::mat4>(), -position) * glm::toMat4(glm::normalize(orientation))
		;//TODO make View produce the same result as tempView!
	glm::mat4 Proj = glm::perspective(glm::radians(vfov), aratio, fp, bp);

	glm::vec3 w = glm::normalize(position - lookAtP);
	glm::vec3 u = glm::normalize(glm::cross(viewUp, w));
	glm::vec3 v = glm::cross(w, u);
	glm::mat4 tempView = 
		glm::mat4(u.x, v.x, w.x, 0,
				u.y, v.y, w.y, 0,
				u.z, v.z, w.z, 0,
				0, 0, 0, 1)
		* glm::translate(glm::identity<glm::mat4>(), -position)
		 ;

	program->activate();
	program->setUniform("camera.wPos", &position);
	glm::mat4 VP = Proj * View;
	program->setUniform("camera.VP", &VP);
}

void Camera::setParams(float _vfov, float _aratio, float _fp, float _bp) {
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

void Camera::rotate(glm::vec3 radiansPerAxis) {
	glm::quat rotQuat(radiansPerAxis);
	rotQuat = glm::normalize(rotQuat);
	orientation *= rotQuat;
	orientation = glm::normalize(orientation);
}

void Camera::lookAt(glm::vec3 point) {
	lookAtP = point; return;//TODO make quat work
	orientation = glm::normalize(glm::quatLookAt(position-point, viewUp));
}

void Camera::orbit(glm::vec3 point, glm::vec3 radiansPerAxis) {
	glm::quat rotQuat(radiansPerAxis);
	rotQuat = glm::normalize(rotQuat);
	position = point + (rotQuat * (position - point));
}