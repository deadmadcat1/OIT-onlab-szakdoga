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

unsigned int Shader::create(const char* const path) {
	shaderID = glCreateShader(type);
	if (!shaderID) {
		fprintf(stderr, "Shader creation failed: %s\n", path);
		glDeleteShader(shaderID);
		return NULL;
	}
	ifstream sourcefile(path);
	if (!sourcefile.is_open()) {
		fprintf(stderr, "Cannot open file: %s\n", path);
		glDeleteShader(shaderID);
		sourcefile.close();
		return NULL;
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
		return -1;
	}
	source[size] = '\0';

	glShaderSource(shaderID, 1, (const GLchar**)&source, NULL);
	glCompileShader(shaderID);
	return shaderID;
}