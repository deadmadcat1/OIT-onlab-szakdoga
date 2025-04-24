#include "GPUProgram.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>

using namespace glm;

unsigned int GPUProgram::getID() {
	return progID;
}

void GPUProgram::getErrorInfo() {
	int logLen, written;
	glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		std::string log;
		log.resize(logLen + 1);
		glGetProgramInfoLog(progID, logLen, &written, &log[0]);
		std::cerr << "Program log:" << std::endl << log << std::endl;
	}
	else {
		std::cerr << "No program log available." << std::endl;
	}
}

bool GPUProgram::checkLinking() {
	int OK;
	glGetProgramiv(progID, GL_LINK_STATUS, &OK);
	if (!OK){
		std::cerr << "Failed to link GPU program!" << std::endl;
		getErrorInfo();
	}
	return OK;
}

int GPUProgram::getLocation(const std::string& name) {
	int location = glGetUniformLocation(progID, name.c_str());
	if (location < 0) std::cerr << "Uniform " << name << " cannot be set!" << std::endl;
	return location;
}

void GPUProgram::addShader(std::shared_ptr<Shader> shader) {
	shaders.push_back(shader);
}

bool GPUProgram::create(const std::string& fragOut) {
	progID = glCreateProgram();
	if (!progID) {
		std::cerr << "Error in shader program creation" << std::endl;
		return false;
	}

	for (std::shared_ptr<Shader> s: shaders) {
		glAttachShader(progID, s->getID());
	}

	glBindFragDataLocation(progID, 0, fragOut.c_str());

	glLinkProgram(progID);
	if (!checkLinking()) return false;
	activate();
	return true;
}

void GPUProgram::activate() {
	glUseProgram(progID);
}

void GPUProgram::setUniform(int i, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1i(location, i);
}

void GPUProgram::setUniform(float f, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1f(location, f);
}

void GPUProgram::setUniform(const vec2* const v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform2fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const vec3* const v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform3fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const vec4* const v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform4fv(location, 1, &v->x);
}

void GPUProgram::setUniform(const mat4* const mat, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, &(*mat)[0][0]);
}

void GPUProgram::setUniform(const Texture* const texture,
	const std::string& samplerName,
	unsigned int textureUnit) {

	int location = getLocation(samplerName);
	if (location >= 0) {
		glUniform1i(location, textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture->getID());
	}
}

GPUProgram::~GPUProgram() { if (progID > 0) glDeleteProgram(progID); }