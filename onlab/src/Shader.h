#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
	unsigned int shaderID = 0;
	GLenum type;
public:
	explicit Shader(GLenum shaderType);
	unsigned int getID();
	void getErrorInfo();
	bool create(const std::string path);
};