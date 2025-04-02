#include "Object.h"

Object::Object(std::unique_ptr<Material>& _material, std::unique_ptr<Geometry>& _geometry){
	materials.push_back(_material);
	geometry = std::move(_geometry);
}

void Object::addMaterial(std::unique_ptr<Material>& _material) {
	materials.push_back(_material);
}

glm::mat4 Object::modelMatrix() const {
	glm::mat4 identity;
	return glm::scale(identity, scaling)
		* glm::rotate(identity, orientation.x, glm::vec3(1, 0, 0))
		* glm::rotate(identity, orientation.y, glm::vec3(0, 1, 0))
		* glm::rotate(identity, orientation.z, glm::vec3(0, 0, 1))
		* glm::translate(identity, position);
}

glm::mat4 Object::modelMatrixInverse() const {
	glm::mat4 identity;
	return glm::translate(identity, -position)
		* glm::rotate(identity, -orientation.z, glm::vec3(0, 0, 1))
		* glm::rotate(identity, -orientation.y, glm::vec3(0, 1, 0))
		* glm::rotate(identity, -orientation.x, glm::vec3(1, 0, 0))
		* glm::scale(identity, glm::vec3(1,1,1) / scaling);
}

void Object::translate(glm::vec3 amountPerAxis) {
	position += amountPerAxis;
}

void Object::rotate(glm::vec3 amountPerAxis) {
	glm::quat rotQuat(orientation);
	rotQuat = glm::rotate(rotQuat, amountPerAxis.x, glm::vec3(1, 0, 0))
			* glm::rotate(rotQuat, amountPerAxis.y, glm::vec3(0, 1, 0))
			* glm::rotate(rotQuat, amountPerAxis.z, glm::vec3(0, 0, 1));
	orientation *= rotQuat;
}

void Object::scale(glm::vec3 amountPerAxis) {
	//TODO
}


void Object::draw() const {
	for (const std::unique_ptr<Material>& mat : materials) {
		mat.get()->bindUniforms();
	}
	geometry->draw();
}