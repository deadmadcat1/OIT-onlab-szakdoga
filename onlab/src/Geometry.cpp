#include "Geometry.h"
#include <iostream>

Geometry::Geometry() {
	VAO = 0;
	buffers.fill(0);
}

bool Geometry::create() {
	glGenVertexArrays(1, &VAO);
	if (!VAO) {
		std::cerr << "VertexArrayObject creation failed!!!" << std::endl;
		return false;
	}
	glBindVertexArray(VAO);
	glGenBuffers(4, buffers.data());
	for (int i = 0; i < 4; i++)
	{
		if (!buffers[i]) {
			std::cerr << "Buffer "<< i << " creation failed!!!" << std::endl;
			glDeleteBuffers(4, buffers.data());
			glBindVertexArray(NULL);
			glDeleteVertexArrays(1, &VAO);
			return false;
		}
	}
	return true;
}

Geometry::~Geometry() {
	glDeleteBuffers(4, buffers.data());
	glBindVertexArray(NULL);
	glDeleteVertexArrays(1, &VAO);
}