#pragma once
#include <GL/glew.h>
#include <string>

// The Texture2d class is a 2D, RGB texture - basically an image.
class Texture2d {
	private:
	GLuint texture;

	public:
	Texture2d(const std::string &path, GLint format = GL_RGB);
	~Texture2d();
	void bind(int location) const;
};