#include "Object.h"
#include <glm/ext/matrix_transform.hpp>

Object::Object(std::shared_ptr<Geometry> _geometry) {
	geometry = _geometry;
}

Object::Object(std::shared_ptr<Material> _material, std::shared_ptr<Geometry> _geometry){
	addMaterial(_material);
	geometry = _geometry;
}

void Object::addMaterial(std::shared_ptr<Material> _material) {
	materials.push_back(_material);
}

void Object::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 M = modelMatrix();
	glm::mat4 Minv = modelMatrixInverse();
	program->activate();
	program->setUniform("object.M", &M);
	program->setUniform("object.Minv", &Minv);

	for (std::shared_ptr<Material> mat : materials) {
		mat->bindUniforms(program);
	}
}

glm::mat4 Object::modelMatrix() const {
	glm::mat4 identity(glm::mat4(1.0f));
	glm::mat4 M = glm::translate(identity, position)
				* glm::toMat4(glm::normalize(orientation))
				* glm::scale(identity, scaling);
	return M;
}

glm::mat4 Object::modelMatrixInverse() const {
	glm::mat4 identity(1.0f);
	glm::mat4 Minv = glm::scale(identity, glm::vec3(1) / scaling)
					* glm::toMat4(glm::conjugate(glm::normalize(orientation)))
					* glm::translate(identity, -position);
	return Minv;
}

void Object::translate(glm::vec3 amountPerAxis) {
	position += amountPerAxis;
}

void Object::rotate(glm::vec3 degreesPerAxis) {
	glm::quat rotQuat(glm::radians(degreesPerAxis));
	rotQuat = glm::normalize(rotQuat);
	orientation *= rotQuat;
	orientation = glm::normalize(orientation);
}

void Object::scale(glm::vec3 amountPerAxis) {
	scaling *= amountPerAxis;
}

void Object::draw() const{
	geometry->draw();
}