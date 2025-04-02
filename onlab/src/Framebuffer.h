#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "Texture.h"

class Framebuffer{
	unsigned int framebufferID = -1;
	unsigned int depthbufferID = -1;
	std::vector<std::unique_ptr<Texture>> targets;
	unsigned int _w = 512;
	unsigned int _h = 512;
	unsigned int _format = GL_RGBA;
public:
	unsigned int getID();
	unsigned int create(unsigned int targetCount = 1,
		unsigned int width = 512,
		unsigned int height = 512,
		unsigned int format = GL_RGBA,
		unsigned int sourceFormat = GL_RGBA,
		unsigned int sourceType = GL_UNSIGNED_BYTE);

	void bind();
	~Framebuffer();
};