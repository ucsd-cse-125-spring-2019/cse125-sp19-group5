#include "Texture2d.h"
#include <iostream>
#include <soil.h>

Texture2d::Texture2d(const std::string &path, GLint format): texture(0) {
	// Load the image data from the path to the image into pixels.
	int width, height, channels;

	auto pixels = SOIL_load_image(
		path.c_str(),
		&width, &height, &channels,
		SOIL_LOAD_RGB
	);
	if (!pixels) {
		std::cerr << "Texture2d(" << path << ") failed to load!" << std::endl
			<< SOIL_last_result() << std::endl;
		return;
	}

	// Then generate a 2D texture, and load the pixel data into it.
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0, format,
		width, height,
		0, format,
		GL_UNSIGNED_BYTE,
		pixels
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up.
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(pixels);
	pixels = nullptr;
}

void Texture2d::bind(int location) const {
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, texture);
}

Texture2d::~Texture2d() {
	if (texture) {
		glDeleteTextures(1, &texture);
		texture = 0;
	}
}