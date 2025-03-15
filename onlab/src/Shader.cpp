#include "Shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Shader::Shader(GLenum shaderType, const char* const path) {
	this->type = shaderType;
	this->pathToSource = path;
}

unsigned int Shader::getID() {
	return (shaderID > -1) ? shaderID : NULL;
}

unsigned int Shader::create() {
	unsigned int shader = glCreateShader(type);
	if (!shader) {
		fprintf(stderr, "Shader creation failed: %s\n", pathToSource);
		glDeleteShader(shader);
		return -1;
	}
	ifstream sourcefile(pathToSource);
	if (!sourcefile.is_open()) {
		fprintf(stderr, "Cannot open file: %s\n", pathToSource);
		glDeleteShader(shader);
		sourcefile.close();
		return -1;
	}
	sourcefile.seekg(ios::end);
	streamsize size = sourcefile.tellg();
	sourcefile.seekg(ios::beg);
	char* source = new char[size + 1];

	if (!sourcefile.read(source, size)) {
		fprintf(stderr, "Error reading file: %s\n", pathToSource);
		glDeleteShader(shader);
		sourcefile.close();
		delete[] source;
		return -1;
	}
	source[size] = '\0';

	glShaderSource(shader, 1, (const GLchar**)&source, NULL);
	glCompileShader(shader);
	return shader;
}