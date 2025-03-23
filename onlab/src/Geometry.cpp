#include "Geometry.h"

Geometry::Geometry() {
	VAO = -1;
	buffers.reserve(4);
}

int Geometry::create() {
	glGenVertexArrays(1, &VAO);
	if (!VAO) {
		fprintf(stderr, "VertexArrayObject creation failed!!!\n");
		return NULL;
	}
	glBindVertexArray(VAO);
	glGenBuffers(4, buffers.data());
	for (int i = 0; i < 4; i++)
	{
		if (!buffers[i]) {
			fprintf(stderr, "Buffer %d creation failed!!!\n", i);
			glDeleteBuffers(4, buffers.data());
			glBindVertexArray(NULL);
			glDeleteVertexArrays(1, &VAO);
			return NULL;
		}
	}
}

Geometry::~Geometry() {
	glDeleteBuffers(4, buffers.data());
	glBindVertexArray(NULL);
	glDeleteVertexArrays(1, &VAO);
}