#pragma once
#include <GL/glew.h>
#include <string>

using TextureParams = struct TextureParams{
	GLenum min_filter;
	GLenum mag_filter;
	GLenum wrap_s;
	GLenum wrap_t;
	

	TextureParams(
		GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR,
		GLenum mag_filter = GL_LINEAR,
		GLenum wrap_s = GL_REPEAT,
		GLenum wrap_t = GL_REPEAT
		) : min_filter(min_filter),
		mag_filter(mag_filter),
		wrap_s(wrap_s),
		wrap_t(wrap_t){}
};

class Texture {
	unsigned int textureID = 0;
public:
	unsigned int getID();
	unsigned int getID() const;
	bool create(const std::string& path, const TextureParams opt);
	bool create(
		unsigned int width,
		unsigned int height,
		unsigned int format,
		unsigned int sourceFormat,
		unsigned int sourceType,
		const TextureParams opt);
	~Texture();
};