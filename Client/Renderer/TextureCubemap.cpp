#include "TextureCubemap.h"

TextureCubemap::TextureCubemap(const std::vector<std::string> &facePaths) {
}

void TextureCubemap::bind() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}