#pragma once
#include <GL/glew.h>

class Shader {
	unsigned int shaderID = -1;
	GLenum type;
public:
	explicit Shader(GLenum shaderType);
	unsigned int getID();
	void getErrorInfo(unsigned int handle);
	bool create(const char* const path);
};