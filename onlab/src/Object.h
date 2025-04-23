#pragma once
#include <vector>
#include <memory>
#include "GPUProgram.h"
#include "Geometry.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Object {
	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<Geometry> geometry;
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::quat orientation = glm::quat(1, 0, 0, 0);
	glm::vec3 scaling = glm::vec3(1, 1, 1);
public:
	Object(std::shared_ptr<Material>_material,
		   std::shared_ptr<Geometry> _geometry);

	void addMaterial(std::shared_ptr<Material> material);

	glm::mat4 modelMatrix() const;

	glm::mat4 modelMatrixInverse() const;

	void translate(glm::vec3 amountPerAxis);
	
	void rotate(glm::vec3 amountPerAxis);

	void scale(glm::vec3 amountPerAxis);

	void draw() const;
};