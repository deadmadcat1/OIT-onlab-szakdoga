#include "Object.h"

Object::Object(Material* _material, Geometry* _geometry) : geometry(_geometry) {
	materials.push_back(_material);
}

void Object::addMaterial(Material* _material) {
	materials.push_back(_material);
}

const glm::mat4 Object::modelMatrix() const {
	glm::mat4 identity;
	return glm::scale(identity, scale)
		* glm::rotate(identity, orientation.x, glm::vec3(1, 0, 0))
		* glm::rotate(identity, orientation.y, glm::vec3(0, 1, 0))
		* glm::rotate(identity, orientation.z, glm::vec3(0, 0, 1))
		* glm::translate(identity, position);
}

const glm::mat4 Object::modelMatrixInverse() const {
	glm::mat4 identity;
	return glm::translate(identity, -position)
		* glm::rotate(identity, -orientation.z, glm::vec3(0, 0, 1))
		* glm::rotate(identity, -orientation.y, glm::vec3(0, 1, 0))
		* glm::rotate(identity, -orientation.x, glm::vec3(1, 0, 0))
		* glm::scale(identity, glm::vec3(1,1,1) / scale);
}

void Object::draw() const {
	for (Material* mat : materials) {
		mat->bindUniforms();
	}
	geometry->draw();
}