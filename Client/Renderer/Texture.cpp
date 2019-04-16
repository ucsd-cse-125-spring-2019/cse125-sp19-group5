#include "Texture.h"
#include <soil.h>
#include <iostream>

void Texture::loadTextureImage(
	GLenum target,
	const std::string &path,
	GLenum format,
	GLenum intformat,
	GLenum type
) {
	// Load image from the path to file into pixels.
	int width, height, channels;
	auto pixels = SOIL_load_image(
		path.c_str(),
		&width, &height, &channels,
		SOIL_LOAD_RGB
	);
	if (!pixels) {
		std::cerr << "Texture::loadTextureImage(" << path << ") failed!"
			<< std::endl << SOIL_last_result() << std::endl;
		return;
	}

	// Assuming a texture is already bound, load the image data into it.
	glTexImage2D(target, 0, intformat, width, height, 0, format, type, pixels);
	glGenerateMipmap(target);

	// Then, clean up the loaded data as it is now in the texture.
	SOIL_free_image_data(pixels);
	pixels = nullptr;
}

Texture::Texture(): texture(0) {
	glGenTextures(1, &texture);
}

Texture::~Texture() {
	if (texture) {
		glDeleteTextures(1, &texture);
		texture = 0;
	}
}