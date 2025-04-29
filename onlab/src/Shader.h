#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

class Shader {
	unsigned int shaderID = 0;
	std::vector<std::string> defines;
	GLenum type;
public:
	explicit Shader(GLenum shaderType);
	unsigned int getID();
	void getErrorInfo();
	void addDefine(std::string directive);
	bool create(const std::string path);
};