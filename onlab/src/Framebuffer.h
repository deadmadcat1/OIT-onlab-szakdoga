#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include "Texture.h"
#include "GPUProgram.h"

class Framebuffer{
	unsigned int framebufferID;
	std::vector<std::shared_ptr<Texture>> colorTargets;
	std::vector<std::string> samplerNames;
	std::shared_ptr<Texture> depthTarget;
	unsigned int _w = 512;
	unsigned int _h = 512;
	unsigned int _format = GL_RGBA;
public:
	unsigned int getID();
	bool create(
		unsigned int targetCount = 1,
		unsigned int width = 512,
		unsigned int height = 512,
		unsigned int format = GL_RGBA,
		unsigned int sourceFormat = GL_RGBA,
		unsigned int sourceType = GL_UNSIGNED_BYTE);
	void bind();
	void activate(unsigned int target);
	void bindUniforms(const std::shared_ptr<GPUProgram>& program);
	~Framebuffer();
};