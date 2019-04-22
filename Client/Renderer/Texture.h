#pragma once

#include <GL/glew.h>
#include <string>

class Texture {
	protected:
	GLuint texture; // ID of the texture for OpenGL.

	// Loads the image data from a given path to an image file and loads it
	// into the target texture.
	void loadTextureImage(
		GLenum target,
		const std::string &path,
		GLenum format = GL_RGB,
		GLenum intformat = GL_RGB,
		GLenum type = GL_UNSIGNED_BYTE
	);

	public:
	Texture();
	void bind(GLenum target = GL_TEXTURE_2D) const;
	GLuint getID() const;
	virtual ~Texture();
};