#include "Shader.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

Shader::Shader(GLenum shaderType) {
	this->type = shaderType;
}

unsigned int Shader::getID() {
	return shaderID;
}

void Shader::getErrorInfo() {
	int logLen, written;
	glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		std::string log;
		log.resize(logLen + 1);
		glGetProgramInfoLog(shaderID, logLen, &written, &log[0]);
		std::cerr << "Shader log:" << std::endl << log << std::endl;
	}
	else {
		std::cerr << "No shader log available." << std::endl;
	}
}

void Shader::addDefine(std::string directive) {
	defines.push_back(directive);
}

bool Shader::create(std::string path) {
	shaderID = glCreateShader(type);
	if (!shaderID) {
		std::cerr << "Shader creation failed: " << path << std::endl;
		return false;
	}
	std::ifstream sourcefile(path);
	if (!sourcefile.is_open()) {
		std::cerr << "Cannot open file: " << path << std::endl;
		glDeleteShader(shaderID);
		sourcefile.close();
		return false;
	}

	std::string content;
	std::string line;
	int lineNum = 0;
	while (std::getline(sourcefile, line)) {
		content += line +'\n';
		if (3 == ++lineNum) {
			for (std::string def : defines) {
				content += def;// +'\n';
			}
			content += "\n";
		}
	}
	content += '\0';

	sourcefile.close();

	auto source = content.c_str();

	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);
	
	int OK;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &OK);
	if (!OK) {
		std::cerr << "Shader compilation error!" << std::endl;
		getErrorInfo();
	}
	
	return true;
}