#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::getID() {
	return framebufferID;
}

unsigned int Framebuffer::create(
	unsigned int targetCount,
	unsigned int width,
	unsigned int height,
	unsigned int format,
	unsigned int sourceFormat,
	unsigned int sourceType)
{
	
	if (framebufferID > -1) {
		std::cerr << "Framebuffer " << framebufferID << " already exists, make a new one!" << std::endl;
		return NULL;
	}
	glGenFramebuffers(1, &framebufferID);
	if (!framebufferID) {
		std::cerr << "Framebuffer creation failed!" << std::endl;
		glDeleteFramebuffers(1, &framebufferID);
		return NULL;
	}
	glGenRenderbuffers(1, &depthbufferID);
	if (!depthbufferID) {
		std::cerr << "Depth buffer creation failed!" << std::endl;
		glDeleteRenderbuffers(1, &depthbufferID);
		glDeleteFramebuffers(1, &framebufferID);
		return NULL;
	}
	
	_w = width;
	_h = height;
	_format = format;

	for (size_t i = 0; i < targetCount; i++)
	{
		auto target = std::make_unique<Texture>();
		target->create(width, height, format, sourceFormat, sourceType, TextureParams(GL_LINEAR));
		targets.push_back(std::move(target));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	std::vector<GLenum> attachments;
	attachments.resize(targetCount);
	for (unsigned int i = 0; i < targetCount; i++)
	{
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(targetCount, attachments.data());

	for (unsigned int i = 0; i < targetCount; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, targets[i]->getID(), 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, depthbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferID);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return framebufferID;
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbufferID);
	GLint viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	_w = viewportDimensions[2];
	_h = viewportDimensions[3];
}

Framebuffer::~Framebuffer(){
	targets.clear();
	glDeleteRenderbuffers(1, &depthbufferID);
	glDeleteFramebuffers(1, &framebufferID);
}