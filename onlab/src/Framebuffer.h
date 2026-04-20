#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <memory>
#include <string>
#include "Texture.h"
#include "GPUProgram.h"

struct colorTargetParameters {
	unsigned int width;
	unsigned int height;
	unsigned int internalFormat;
	unsigned int format;
	unsigned int type;
	const void * data;
	colorTargetParameters(
		unsigned int width,
		unsigned int height,
		unsigned int internalFormat,
		unsigned int format,
		unsigned int type,
		const void* data)
		:width(width), height(height),
		internalFormat(internalFormat),
		format(format),
		type(type), data(data) {}
};

class Framebuffer{
	unsigned int framebufferID;
	std::unordered_map<std::string, std::shared_ptr<Texture>> colorTargets;
	std::unordered_map<std::string, colorTargetParameters> colorTargetParams;
	std::shared_ptr<Texture> depthTarget;
	unsigned int depth_width;
	unsigned int depth_height;

	void createColorTargets();
	void createDepthTarget();
	void attachTextures();
public:
	unsigned int getID();
	bool create(
		const std::unordered_map<std::string, colorTargetParameters>& targetParams,
		unsigned int depthbuffer_width,
		unsigned int depthbuffer_height);
	void bind();
	Texture* getColorTarget(std::string name);
	Texture* getDepthTarget();
	void resize(unsigned int w, unsigned int h);
	void bindUniforms(const std::shared_ptr<GPUProgram>& program);
	~Framebuffer();
};
