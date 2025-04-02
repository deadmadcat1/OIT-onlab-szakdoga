#include "Framebuffer.h"
#include <stdio.h>

unsigned int Framebuffer::getID() {
	return (framebufferID> -1) ? framebufferID: NULL;
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
		fprintf(stderr, "Framebuffer %d already exists, make a new one!\n", framebufferID);
		return NULL;
	}
	glGenFramebuffers(1, &framebufferID);
	if (!framebufferID) {
		fprintf(stderr, "Framebuffer creation failed!\n");
		glDeleteFramebuffers(1, &framebufferID);
		return framebufferID;
	}
	glGenRenderbuffers(1, &depthbufferID);
	if (!depthbufferID) {
		fprintf(stderr, "Depth buffer creation failed!\n");
		glDeleteRenderbuffers(1, &depthbufferID);
		glDeleteFramebuffers(1, &framebufferID);
		return framebufferID;
	}
	
	_w = width;
	_h = height;
	_format = format;

	for (size_t i = 0; i < targetCount; i++)
	{
		std::unique_ptr<Texture> target = std::make_unique<Texture>();
		target.get()->create(width, height, format, sourceFormat, sourceType, TextureParams(GL_LINEAR));
		targets.push_back(target);
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