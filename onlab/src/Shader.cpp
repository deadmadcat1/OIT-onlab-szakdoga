#include "Shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Shader::Shader(GLenum shaderType) {
	this->type = shaderType;
}

unsigned int Shader::getID() {
	return (shaderID > -1) ? shaderID : NULL;
}

void Shader::getErrorInfo(unsigned int handle) {
	int logLen, written;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		char* log = new char[logLen + 1];
		log[logLen] = '\0';
		glGetShaderInfoLog(handle, logLen, &written, &log[0]);
		printf("Shader log:\n%s", log);
		delete[] log;
	}
}

bool Shader::create(const char* const path) {
	shaderID = glCreateShader(type);
	if (!shaderID) {
		fprintf(stderr, "Shader creation failed: %s\n", path);
		glDeleteShader(shaderID);
		return false;
	}
	ifstream sourcefile(path);
	if (!sourcefile.is_open()) {
		fprintf(stderr, "Cannot open file: %s\n", path);
		glDeleteShader(shaderID);
		sourcefile.close();
		return false;
	}
	sourcefile.seekg(ios::end);
	streamsize size = sourcefile.tellg();
	sourcefile.seekg(ios::beg);
	char* source = new char[size + 1];

	if (!sourcefile.read(source, size)) {
		fprintf(stderr, "Error reading file: %s\n", path);
		glDeleteShader(shaderID);
		sourcefile.close();
		delete[] source;
		return false;
	}
	source[size] = '\0';

	glShaderSource(shaderID, 1, (const GLchar**)&source, NULL);
	glCompileShader(shaderID);

	int OK;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &OK);
	if (!OK) { printf("%s!\n", "Shader compilation error"); getErrorInfo(shaderID); getchar(); }
	
	return true;
}