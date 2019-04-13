#include "TextureCubemap.h"
#include <iostream>

constexpr auto NUM_CUBE_FACES = 6;

TextureCubemap::TextureCubemap(const std::vector<std::string> &facePaths) {
	if (facePaths.size() != NUM_CUBE_FACES) {
		std::cerr << "TextureCubemap(facePaths): facePaths size is "
			<< facePaths.size() << " != " << NUM_CUBE_FACES << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < NUM_CUBE_FACES; i++) {
		loadTextureImage(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			facePaths[i]
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCubemap::bind() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}