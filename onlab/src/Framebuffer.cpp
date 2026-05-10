#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::getID() {
	return _framebufferID;
}

bool Framebuffer::create(glm::uvec2 dimensions, const std::unordered_map<std::string, TargetParams>& targetParams) {
	if (_framebufferID > 0) {
		std::cerr << "Framebuffer " << _framebufferID << " already exists, make a new one!" << std::endl;
		return false;
	}

	glGenFramebuffers(1, &_framebufferID);
	if (!_framebufferID) {
		std::cerr << "Framebuffer creation failed!" << std::endl;
		glDeleteFramebuffers(1, &_framebufferID);
		return false;
	}

	bool hasDepthTarget = false;
	for (const auto& tp : targetParams) {
		switch(tp.second.internalFormat) {
			case(GL_DEPTH_COMPONENT):
			case(GL_DEPTH_COMPONENT32F):
			case(GL_DEPTH_COMPONENT24):
			case(GL_DEPTH_COMPONENT16):
			case(GL_DEPTH_STENCIL):
				hasDepthTarget = true;
				break;
		}
	}

	size_t targetCount = targetParams.size() - (hasDepthTarget ? 1 : 0);
	int maxColorAttachments = 0;	//would be better to create a factory that manages this, but idc
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	if (targetCount > (unsigned int)maxColorAttachments - 1) {
		std::cerr << "Framebuffer " << _framebufferID << " cannot store " << targetCount << " color attachments, only "<< maxColorAttachments<< "!" << std::endl;
		return false;
	}

	_dimensions = dimensions;
	_targetParams = targetParams;
	createTargets();
	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::string error;
		switch(status) {
			case GL_FRAMEBUFFER_UNDEFINED:
				error = "GL_FRAMEBUFFER_UNDEFINED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				error = "GL_FRAMEBUFFER_UNSUPPORTED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
				break;
		}
		std::cerr << "Framebuffer " << _framebufferID << " reports error: " << error << "!" << std::endl;
		glDeleteFramebuffers(1, &_framebufferID);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Framebuffer::createTargets() {
	for (const auto& t : _targetParams) {
		auto colorTarget = std::make_unique<Texture>();
		colorTarget->create(t.second.target, t.second.mipmapLevels, t.second.internalFormat, _dimensions.x, _dimensions.y, t.second.depth, t.second.textureParams);
		_targets.emplace(t.first, std::move(colorTarget));
	}
}

void Framebuffer::attachTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);

	GLenum attach;
	std::vector<GLenum> attachments;
	int i = 0;
	for (const auto& t : _targets) {
		switch(_targetParams[t.first].internalFormat) {
			case(GL_DEPTH_COMPONENT):
			case(GL_DEPTH_COMPONENT32F):
			case(GL_DEPTH_COMPONENT24):
			case(GL_DEPTH_COMPONENT16):
			case(GL_DEPTH_STENCIL):
				attach = GL_DEPTH_ATTACHMENT;
				break;
			default:
				attach = GL_COLOR_ATTACHMENT0 + (unsigned int)i;
				attachments.push_back(attach);
				i++;
				break;
		}
		glFramebufferTexture(GL_FRAMEBUFFER, attach,  t.second->getID(), 0);
	}
	glDrawBuffers((unsigned int)attachments.size(), attachments.data());
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);
}

void Framebuffer::bindUniforms(const std::shared_ptr<GPUProgram>& program) {
	program->activate();
	int i = 0;
	for (const auto& n : program->sampler_names) {
		if (_targets.find(n) != _targets.end()) {
			program->setUniform(n, _targets[n].get(), (unsigned int)i);
		}
		//else {
		//	std::cout << "Framebuffer " << _framebufferID << " does not produce " << n << "!" << std::endl;
		//}
		i++;
	}
}

Texture* Framebuffer::getTarget(std::string name) {
	return _targets[name].get();
}

Framebuffer::~Framebuffer() {
	_targets.clear();
	glDeleteFramebuffers(1, &_framebufferID);
}

void Framebuffer::resize(glm::uvec2 newsize) {
	_targets.clear();

	_dimensions = newsize;
	createTargets();
	attachTextures();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete!" << std::endl;
	}
} 
