#include "Object.h"

Object::Object(std::shared_ptr<Material> _material, std::shared_ptr<Geometry> _geometry){
	materials.push_back(_material);
	geometry = _geometry;
}

void Object::addMaterial(std::shared_ptr<Material> _material) {
	materials.push_back(_material);
}

void Object::bindUniforms(std::shared_ptr<GPUProgram> program) {
	glm::mat4 M = modelMatrix();
	glm::mat4 Minv = modelMatrixInverse();
	program->activate();
	program->setUniform("M", &M);
	program->setUniform("Minv", &Minv);

	for (std::shared_ptr<Material> mat : materials) {
		mat->bindUniforms(program);
	}
}

glm::mat4 Object::modelMatrix() const {
	glm::mat4 identity;
	return glm::scale(identity, scaling)
		* glm::toMat4(orientation)
		* glm::translate(identity, position);
}

glm::mat4 Object::modelMatrixInverse() const {
	glm::mat4 identity;
	return glm::translate(identity, -position)
		* glm::toMat4(-orientation)
		* glm::scale(identity, glm::vec3(1,1,1) / scaling);
}

void Object::translate(glm::vec3 amountPerAxis) {
	position += amountPerAxis;
}

void Object::rotate(glm::vec3 amountPerAxis) {
	glm::quat rotQuat(amountPerAxis);
	orientation *= rotQuat;
}

void Object::scale(glm::vec3 amountPerAxis) {
	scaling *= amountPerAxis;
}

void Object::draw() const{
	geometry->draw();
}