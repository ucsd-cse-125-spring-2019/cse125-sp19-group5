#include "Texture2d.h"
#include <iostream>
#include <soil.h>

Texture2d::Texture2d(const std::string &path, GLint format) {
	glBindTexture(GL_TEXTURE_2D, texture);
	loadTextureImage(GL_TEXTURE_2D, path, format);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2d::bind(int location) const {
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, texture);
}
