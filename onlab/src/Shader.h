#pragma once
#include <GL/glew.h>

class Shader {
	unsigned int shaderID = -1;
	GLenum type;
public:
	Shader(GLenum shaderType);
	unsigned int getID();
	unsigned int create(const char* const path);
};