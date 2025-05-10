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

	_w = width;
	_h = height;
	_format = format;

	for (size_t i = 0; i < targetCount; i++)
	{
		auto colorTarget = std::make_unique<Texture>();
		colorTarget->create(width, height, format, sourceFormat, sourceType, TextureParams(GL_LINEAR));
		colorTargets.push_back(std::move(colorTarget));
	}
	depthTarget = std::make_unique<Texture>();
	depthTarget->create(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, TextureParams(GL_LINEAR));

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	std::vector<GLenum> attachments;
	attachments.reserve(targetCount);
	for (unsigned int i = 0; i < targetCount; i++)
	{
		unsigned int attach = GL_COLOR_ATTACHMENT0 + i;
		attachments.push_back(attach);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, colorTargets[i]->getID(), 0);
	}
	glDrawBuffers(targetCount, attachments.data());

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getID(), 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
		glDeleteFramebuffers(1, &framebufferID);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glViewport(0, 0, _w, _h);
}

void Framebuffer::activate(unsigned int target) {
	glBindTexture(GL_TEXTURE_2D, colorTargets[target]->getID());
}

void Framebuffer::bindUniforms(const std::shared_ptr<GPUProgram>& program) {
	program->activate();
	program->setUniform("depthSampler", depthTarget.get(), 0);
	for (int i = 0; i < colorTargets.size(); i++) {
		program->setUniform("colorSampler" + std::to_string(i+1), colorTargets[i].get(), i+1);
	}
}

Framebuffer::~Framebuffer(){
	colorTargets.clear();
	glDeleteFramebuffers(1, &framebufferID);
}