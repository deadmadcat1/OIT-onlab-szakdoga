#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::getID() {
	return framebufferID;
}

bool Framebuffer::create(const std::unordered_map<std::string, TargetParams>& targetParams) {
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

	bool hasDepthTarget = false;
	for (const auto& tp : targetParams) {
		if (tp.second.internalFormat == GL_DEPTH_COMPONENT) {
			hasDepthTarget = true;
		}
	}

	size_t targetCount = targetParams.size() - (hasDepthTarget ? 1 : 0);
	int maxColorAttachments = 0;	//would be better to create a factory that manages this, but idc
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	if (targetCount > (unsigned int)maxColorAttachments - 1) {
		std::cerr << "Framebuffer " << framebufferID << " cannot store " << targetCount << " color attachments, only "<< maxColorAttachments<< "!" << std::endl;
		return false;
	}

	_targetParams = targetParams;
	createTargets();
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

void Framebuffer::createTargets() {
	for (const auto& t : _targetParams) {
		auto colorTarget = std::make_unique<Texture>();
		colorTarget->create(t.second.width, t.second.height, t.second.internalFormat, t.second.format, t.second.type, t.second.data, TextureParams(GL_LINEAR));
		_targets.emplace(t.first, std::move(colorTarget));
	}
}

void Framebuffer::attachTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	GLenum attach;
	std::vector<GLenum> attachments;
	int i = 0;
	for (const auto& t : _targets) {
		if (_targetParams[t.first].internalFormat == GL_DEPTH_COMPONENT) {
			attach = GL_DEPTH_ATTACHMENT;
		} else {
			attach = GL_COLOR_ATTACHMENT0 + (unsigned int)i;
			attachments.push_back(attach);
			i++;
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, t.second->getID(), 0);
	}
	glDrawBuffers((unsigned int)attachments.size(), attachments.data());
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void Framebuffer::bindUniforms(const std::shared_ptr<GPUProgram>& program) {
	program->activate();
	int i = 0;
	for (const auto& n : program->sampler_names) {
		if (_targets.find(n) != _targets.end()) {
			program->setUniform(n, _targets[n].get(), (unsigned int)i);
		}
		//else {
		//	std::cout << "Framebuffer " << framebufferID << " does not produce " << n << "!" << std::endl;
		//}
		i++;
	}
}

Texture* Framebuffer::getTarget(std::string name) {
	return _targets[name].get();
}

Framebuffer::~Framebuffer() {
	_targets.clear();
	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::resize(glm::uvec2 newsize) {
	_targets.clear();

	for (auto& tp : _targetParams) {
		tp.second.width = newsize.x;
		tp.second.height = newsize.y;
	}
	createTargets();

	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
	}
} 
