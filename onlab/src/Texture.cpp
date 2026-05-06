#include "Texture.h"
#include <GL/glew.h>
#include <iostream>
#include "stb_image.h"

unsigned int Texture::getID() {
	return textureID;
}
unsigned int Texture::getID() const {
	return textureID;
}
bool Texture::create(const std::string& path, const TextureParams opt) {
	if (textureID > 0) {
		std::cerr << "Texture " << textureID << " already exists, make a new one!" << std::endl;
		return false;
	}
	glGenTextures(1, &textureID);
	if (!textureID) {
		std::cerr << "Texture creation failed: " << path << std::endl;
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, opt._wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt._wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, opt._min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, opt._mag_filter);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data) {
		GLint gl_channels;
		switch (channels) {
		case 1:
			gl_channels = GL_RED;
			break;
		case 2:
			gl_channels = GL_RG;
			break;
		case 3:
			gl_channels = GL_RGB;
			break;
		case 4:
		default:
			gl_channels = GL_RGBA;
			break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, gl_channels, width, height,
					 0, gl_channels, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "Unable to read image data: " << path << std::endl;
		glDeleteTextures(1, &textureID);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	
	return true;
}

bool Texture::create(
	GLenum target,
	unsigned int mipmapLevels,
	GLenum internalFormat,
	unsigned int width,
	unsigned int height,
	unsigned int depth,
	const TextureParams opt) {
	if (textureID > 0) {
		std::cerr << "Texture " << textureID << " already exists, make a new one!" << std::endl;
		return false;
	}
	glGenTextures(1, &textureID);
	if (!textureID) {
		std::cerr << "Texture creation failed!" << std::endl;
		return false;
	}
	GLint maxLayers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);
	if (target == GL_TEXTURE_2D_ARRAY && depth > (unsigned int)maxLayers) {
		std::cerr << "Maximum supported texture array length is only: " << maxLayers << "!" << std::endl;
		return false;
	}

	glBindTexture(target, textureID);
	
	glTexParameteri(target, GL_TEXTURE_WRAP_S, opt._wrap_s);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, opt._wrap_t);
	if (target == GL_TEXTURE_3D) {
		glTexParameteri(target, GL_TEXTURE_WRAP_R, opt._wrap_r);
	}
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, opt._min_filter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, opt._mag_filter);

	if (target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_3D) {
		glTexStorage3D(target, mipmapLevels, internalFormat, width, height, depth);
	} else {
		glTexStorage2D(target, mipmapLevels, internalFormat, width, height);
	}

	glBindTexture(target, 0);
	return true;
}

Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}
