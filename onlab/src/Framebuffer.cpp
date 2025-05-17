#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::getID() {
	return framebufferID;
}

bool Framebuffer::create(
	unsigned int targetCount,
	unsigned int width,
	unsigned int height,
	unsigned int format,
	unsigned int sourceFormat,
	unsigned int sourceType)
{
	int maxColorAttachments = 0;	//would be better to create a factory that manages this, but idc
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	if (targetCount > maxColorAttachments - 1) {
		std::cerr << "Framebuffer " << framebufferID << " cannot store " << targetCount << " color attachments, only "<< maxColorAttachments<< "!" << std::endl;
		targetCount = maxColorAttachments;
	}
	if (framebufferID > 0) {
		std::cerr << "Framebuffer " << framebufferID << " already exists, make a new one!" << std::endl;
		return false;
	}
	glGenFramebuffers(1, &framebufferID);
	if (!framebufferID) {
		std::cerr << "Framebuffer creation failed!" << std::endl;
		glDeleteFramebuffers(1, &framebufferID);
		return false;
	}
	
	_targetCount = targetCount;
	_width = width;
	_height = height;
	_format = format;
	_sourceFormat = sourceFormat;
	_sourceType = sourceType;

	createTextures(targetCount, width, height, format, sourceFormat, sourceType);

	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
		glDeleteFramebuffers(1, &framebufferID);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Framebuffer::createTextures(unsigned int targetCount,
	unsigned int width,
	unsigned int height,
	unsigned int format,
	unsigned int sourceFormat,
	unsigned int sourceType) {
	for (size_t i = 0; i < targetCount; i++)
	{
		auto colorTarget = std::make_unique<Texture>();
		colorTarget->create(width, height, format, sourceFormat, sourceType, TextureParams(GL_LINEAR));
		colorTargets.push_back(std::move(colorTarget));
	}
	depthTarget = std::make_shared<Texture>();
	depthTarget->create(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, TextureParams(GL_LINEAR));

}

void Framebuffer::attachTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	std::vector<GLenum> attachments;
	for (size_t i = 0; i < _targetCount; i++)
	{
		GLenum attach = GL_COLOR_ATTACHMENT0 + i;
		attachments.push_back(attach);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, colorTargets[i]->getID(), 0);
	}
	glDrawBuffers(attachments.size(), attachments.data());

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getID(), 0);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glViewport(0, 0, _width, _height);
}

void Framebuffer::bindUniforms(const std::shared_ptr<GPUProgram>& program) {
	program->activate();
	program->setUniform("depthSampler", depthTarget.get(), 0);
	for (int i = 0; i < colorTargets.size(); i++) {
		program->setUniform("colorSampler" + std::to_string(i+1), colorTargets[i].get(), i+1);
	}
}

Texture* Framebuffer::getColorTarget(int idx) {
	return colorTargets[idx].get();
}
Texture* Framebuffer::getDepthTarget() {
	return depthTarget.get();
}

Framebuffer::~Framebuffer(){
	colorTargets.clear();
	depthTarget.reset();
	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::resize(int w, int h) {
	colorTargets.clear();
	depthTarget.reset();

	_width = w;
	_height = h;

	createTextures(_targetCount, _width, _height, _format, _sourceFormat, _sourceType);

	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
	}
}
