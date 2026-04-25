#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <memory>
#include <string>
#include "Texture.h"
#include "GPUProgram.h"

struct TargetParams {
	unsigned int width;
	unsigned int height;
	unsigned int internalFormat;
	unsigned int format;
	unsigned int type;
	const void * data;
	TargetParams(
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
	TargetParams() {}
};

class Framebuffer{
	unsigned int framebufferID;
	std::unordered_map<std::string, std::shared_ptr<Texture>> _targets;
	std::unordered_map<std::string, TargetParams> _targetParams;

	void createTargets();
	void attachTextures();
public:
	unsigned int getID();
	bool create(const std::unordered_map<std::string, TargetParams>& colorTargetParams);
	void bind();
	Texture* getTarget(std::string name);
	void resize(glm::uvec2 newsize);
	void bindUniforms(const std::shared_ptr<GPUProgram>& program);
	~Framebuffer();
};
