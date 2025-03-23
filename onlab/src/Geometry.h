#pragma once
#include <stdio.h>
#include <vector>
#include <GL/glew.h>

class Geometry {
public:
	Geometry();
	unsigned int VAO;
	std::vector<unsigned int> buffers;	//VBO, normals, texcoords, indices

	virtual int create();
	virtual ~Geometry();
};