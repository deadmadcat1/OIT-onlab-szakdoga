#pragma once
#include "Geometry.h"
#include "Material.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL	//only for glm::toMat4 to not throw compile error, it is just a wrapper for glm::mat4_cast(glm::quat)
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <memory>

class Object {
	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<Geometry> geometry;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scaling = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 modelMatrix() const;
	glm::mat4 modelMatrixInverse() const;
public:
	Object(std::shared_ptr<Material>_material,
		   std::shared_ptr<Geometry> _geometry);
	void addMaterial(std::shared_ptr<Material> material);
	void bindUniforms(std::shared_ptr<GPUProgram> program);
	void translate(glm::vec3 amountPerAxis);
	void rotate(glm::vec3 amountPerAxis);
	void scale(glm::vec3 amountPerAxis);
	void draw() const;
};