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
	unsigned int _targetCount;
	unsigned int _width;
	unsigned int _height;
	unsigned int _format;
	unsigned int _sourceFormat;
	unsigned int _sourceType;
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
	Texture* getColorTarget(int idx);
	Texture* getDepthTarget();
	void resize(int width, int height);
	void bindUniforms(const std::shared_ptr<GPUProgram>& program);
	void createTextures(unsigned int targetCount,
		unsigned int width,
		unsigned int height,
		unsigned int format,
		unsigned int sourceFormat,
		unsigned int sourceType);
	void attachTextures();
	~Framebuffer();
};