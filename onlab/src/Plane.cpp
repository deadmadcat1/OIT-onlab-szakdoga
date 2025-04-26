#include "Geometry.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

Plane::Plane() : Geometry() {}

bool Plane::create() {
	if (!Geometry::create()) {
		return false;
	}

	/*VERTICES*/
	std::vector<glm::vec3> vertices;

	vertices.push_back(glm::vec3(-1, -1, 0));
	vertices.push_back(glm::vec3(-1, 1, 0));
	vertices.push_back(glm::vec3(1, -1, 0));
	vertices.push_back(glm::vec3(1, 1, 0));

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	/*NORMALS*/
	std::vector<glm::vec3> normals;

	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	/*UVs*/
	std::vector<glm::vec2> uvs;

	uvs.push_back(glm::vec2(0, 0));
	uvs.push_back(glm::vec2(0, 1));
	uvs.push_back(glm::vec2(1, 0));
	uvs.push_back(glm::vec2(1, 1));

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
	/*INDICES*/
	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);

	nIdx = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIdx * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
	/*LAYOUT*/
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3, GL_FLOAT,
		GL_FALSE,
		0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3, GL_FLOAT,
		GL_FALSE,
		0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2, GL_FLOAT,
		GL_FALSE,
		0, NULL);

	glBindVertexArray(NULL);
	return true;
}

void Plane::draw() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);

	glDrawElements(GL_TRIANGLE_STRIP, nIdx, GL_UNSIGNED_SHORT, NULL);
}