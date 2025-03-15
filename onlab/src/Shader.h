#pragma once
#include <GL/glew.h>

class Shader {
	unsigned int shaderID = -1;
	GLenum type;
	const char* pathToSource;
public:
	Shader(GLenum shaderType, const char* const path);
	unsigned int getID();
	unsigned int create();
};