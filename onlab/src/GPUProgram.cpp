#include <GL/glew.h>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include "GPUProgram.h"
#include "Shader.h"

using namespace std;
using namespace glm;

void GPUProgram::getErrorInfo(unsigned int handle) {
	int logLen, written;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		char* log = new char[logLen+1];
		log[logLen] = '\0';
		glGetShaderInfoLog(handle, logLen, &written, &log[0]);
		printf("Shader log:\n%s", log);
		delete[] log;
	}
}

void GPUProgram::checkShader(unsigned int shader, const char* const message) {
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) { printf("%s!\n", message); getErrorInfo(shader); getchar(); }
}

void GPUProgram::checkLinking(unsigned int program) {
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) { printf("Failed to link shader program!\n"); getErrorInfo(program); getchar(); }
}

int GPUProgram::getLocation(const char* const name) {
	int location = glGetUniformLocation(progID, name);
	if (location < 0) printf("uniform %s cannot be set\n", name);
	return location;
}

void GPUProgram::addShader(Shader* shader) {
	shaders.push_back(shader);
}

void GPUProgram::create(const char* const fragOut) {
	progID = glCreateProgram();
	if (!progID) {
		fprintf(stderr, "Error in shader program creation\n");
		return;
	}

	for (Shader* s : shaders) {
		checkShader(s->getID(), "Shader compilation error");
		glAttachShader(progID, s->getID());
	}

	glBindFragDataLocation(progID, 0, fragOut);

	glLinkProgram(progID);
	checkLinking(progID);
	activate();
}

void GPUProgram::activate() {
	glUseProgram(progID);
}

void GPUProgram::setUniform(int i, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1i(location, i);
}

void GPUProgram::setUniform(float f, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1f(location, f);
}

void GPUProgram::setUniform(const vec2* const v, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniform2fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const vec3* const v, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniform3fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const vec4* const v, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniform4fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const mat4* const mat, const char* const name) {
	int location = getLocation(name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, &(*mat)[0][0]);
}

void GPUProgram::setUniform(const Texture* texture,
	const char* const samplerName,
	unsigned int textureUnit) {
	int location = getLocation(samplerName);
	if (location >= 0) {
		glUniform1i(location, textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture->getID());
	}
}

GPUProgram::~GPUProgram() { if (progID > 0) glDeleteProgram(progID); }