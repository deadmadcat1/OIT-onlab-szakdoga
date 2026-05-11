#include "Camera.h"
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

void Camera::bindUniforms(std::shared_ptr<GPUProgram> program) {
  glm::mat4 View = glm::lookAt(_position, _lookat, _viewup);
  glm::mat4 Proj = glm::perspective(glm::radians(_vfov), _aratio, _fp, _bp);

  program->activate();
  program->setUniform("camera.wPos", &_position);
  program->setUniform("camera.V", &View);
  program->setUniform("camera.P", &Proj);
  program->setUniform("camera.near", _fp);
  program->setUniform("camera.far", _bp);
}

void Camera::setParams(float vfov, float aratio, float fp, float bp) {
  _vfov = vfov;
  _aratio = aratio;
  _fp = fp;
  _bp = bp;
}

void Camera::setViewUp(glm::vec3 direction) {
  _viewup = glm::normalize(direction);
}

void Camera::lookAt(glm::vec3 point) {
  _lookat = glm::normalize(point - _position);
}

void Camera::translate(glm::vec3 amountPerAxis) { _position += amountPerAxis; }

void Camera::rotate(glm::vec3 degreesPerAxis) {
  glm::vec3 rad = glm::radians(degreesPerAxis);
  glm::mat4 rotMat =
      glm::rotate(glm::mat4(1.0f), -rad.x, glm::cross(_viewup, _lookat));
  rotMat = glm::rotate(rotMat, rad.y, _viewup);
  rotMat = glm::rotate(rotMat, rad.z, _lookat);
  _lookat = glm::normalize(glm::vec3(rotMat * glm::vec4(_lookat, 0)));
  _viewup = glm::normalize((glm::vec3(rotMat * glm::vec4(_viewup, 0))));
}

void Camera::pan(glm::vec2 amount) {
  glm::mat4 View = glm::lookAt(_position, _lookat, _viewup);
  float hfov = _aratio * _vfov;
  glm::vec3 degreesPerAxis = glm::vec3(
      glm::vec4(-0.5f * _vfov * amount.y, 0.5f * hfov * amount.x, 0.0f, 0.0f) *
      View);
  orbit(glm::vec3(0.0f), degreesPerAxis);
}

void Camera::orbit(glm::vec3 point, glm::vec3 degreesPerAxis) {
  glm::quat rotQuat = glm::normalize(glm::quat(glm::radians(degreesPerAxis)));
  _position = point + (rotQuat *(_position - point) * glm::conjugate(rotQuat));
  _viewup = rotQuat * _viewup * glm::conjugate(rotQuat);
}
