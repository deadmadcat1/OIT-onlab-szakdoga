#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::getID() {
	return framebufferID;
}

bool Framebuffer::create(
	const std::unordered_map<std::string, colorTargetParameters>& targetParams,
	unsigned int depthbuffer_width,
	unsigned int depthbuffer_height)
{
	if (framebufferID > 0) {
		std::cerr << "Framebuffer " << framebufferID << " already exists, make a new one!" << std::endl;
		return false;
	}
	size_t targetCount = targetParams.size();
	int maxColorAttachments = 0;	//would be better to create a factory that manages this, but idc
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	if (targetCount > (unsigned int)maxColorAttachments - 1) {
		std::cerr << "Framebuffer " << framebufferID << " cannot store " << targetCount << " color attachments, only "<< maxColorAttachments<< "!" << std::endl;
		return false;
	}
	glGenFramebuffers(1, &framebufferID);
	if (!framebufferID) {
		std::cerr << "Framebuffer creation failed!" << std::endl;
		glDeleteFramebuffers(1, &framebufferID);
		return false;
	}
	depth_width = depthbuffer_width;
	depth_height = depthbuffer_height;
	colorTargetParams = targetParams;
	
	createColorTargets();
	createDepthTarget();
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

void Framebuffer::createColorTargets() {
	for (const auto& t : colorTargetParams) {
		auto colorTarget = std::make_unique<Texture>();
		colorTarget->create(t.second.width, t.second.height, t.second.internalFormat, t.second.format, t.second.type, t.second.data, TextureParams(GL_LINEAR));
		colorTargets.emplace(t.first, std::move(colorTarget));
	}
}

void Framebuffer::createDepthTarget() {
	depthTarget = std::make_shared<Texture>();
	depthTarget->create(depth_width, depth_height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, TextureParams(GL_LINEAR));
}

void Framebuffer::attachTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	std::vector<GLenum> attachments;
	int i = 0;
	for (const auto& ct : colorTargets) {
		GLenum attach = GL_COLOR_ATTACHMENT0 + (unsigned int)i;
		attachments.push_back(attach);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, ct.second->getID(), 0);
		i++;
	}
	glDrawBuffers((unsigned int)attachments.size(), attachments.data());

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getID(), 0);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void Framebuffer::bindUniforms(const std::shared_ptr<GPUProgram>& program) {
	program->activate();
	program->setUniform("depthSampler", depthTarget.get(), 0);
	int i = 0;
	for (const auto& n : program->color_sampler_names) {
		if (colorTargets.find(n) != colorTargets.end()) {
			program->setUniform(n, colorTargets[n].get(), (unsigned int)i);
		}
		//else {
		//	std::cout << "Framebuffer " << framebufferID << " does not produce " << n << "!" << std::endl;
		//}
		i++;
	}
}

Texture* Framebuffer::getColorTarget(std::string name) {
	return colorTargets[name].get();
}
Texture* Framebuffer::getDepthTarget() {
	return depthTarget.get();
}

Framebuffer::~Framebuffer() {
	colorTargets.clear();
	depthTarget.reset();
	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::resize(unsigned int w, unsigned int h) {
	colorTargets.clear();
	depthTarget.reset();

	depth_width = w;
	depth_height = h;
	for (auto& ctp : colorTargetParams) {
		ctp.second.width = w;
		ctp.second.height = h;
	}
	createColorTargets();
	createDepthTarget();

	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
	}
} 
