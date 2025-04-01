#pragma once
#include <vector>
#include "GPUProgram.h"
#include "Geometry.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Object {
	std::vector<Material*> materials;
	Geometry* geometry;
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;
public:
	Object(Material* _material, Geometry* _geometry);
	void addMaterial(Material* material);
	const glm::mat4 modelMatrix() const;
	const glm::mat4 modelMatrixInverse() const;
	void draw() const;
};