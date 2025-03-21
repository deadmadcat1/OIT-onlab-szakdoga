#include "Texture.h"
#include <stdio.h>
#include <GL/glew.h>
#include "stb_image.h"

unsigned int Texture::getID() {
	return (textureID > -1) ? textureID : NULL;
}
unsigned int Texture::getID() const {
	return (textureID > -1) ? textureID : NULL;
}
unsigned int Texture::create(const char* const path, const TextureParams opt) {
	if (textureID > -1) {
		fprintf(stderr, "Texture %d already exists, make a new one!\n", textureID);
		return NULL;
	}
	glGenTextures(1, &textureID);
	if (!textureID) {
		fprintf(stderr, "Texture creation failed: %s\n", path);
		glDeleteTextures(1, &textureID);
		return textureID;
	}
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, opt.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, opt.mag_filter);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		fprintf(stderr, "Unable to read image data: %s\n", path);
		glDeleteTextures(1, &textureID);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	
	return textureID;
}

unsigned int Texture::create(
	unsigned int width,
	unsigned int height,
	unsigned int format,
	unsigned int sourceFormat,
	unsigned int sourceType,
	const TextureParams opt)
{
	if (textureID > -1) {
		fprintf(stderr, "Texture %d already exists, make a new one!\n", textureID);
		return NULL;
	}
	glGenTextures(1, &textureID);
	if (!textureID) {
		fprintf(stderr, "Render Target Texture creation failed!\n");
		glDeleteTextures(1, &textureID);
		return textureID;
	}
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, opt.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt.wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, opt.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, opt.mag_filter);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, sourceFormat, sourceType, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}