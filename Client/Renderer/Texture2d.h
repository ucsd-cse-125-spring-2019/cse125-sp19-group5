#pragma once
#include <GL/glew.h>
#include <string>
#include "Texture.h"

// The Texture2d class is a 2D, RGB texture - basically an image.
class Texture2d : public Texture {
	public:
	Texture2d(const std::string &path, GLint format = GL_RGB);
	void bind(int location) const;
};