#include "Texture.h"
#include <stdio.h>

unsigned int Texture::getID() {
	return (textureID > -1) ? textureID : NULL;
}
unsigned int Texture::getID() const {
	return (textureID > -1) ? textureID : NULL;
}