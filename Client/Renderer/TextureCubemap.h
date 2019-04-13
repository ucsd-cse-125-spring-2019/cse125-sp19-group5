#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include "Texture.h"

class TextureCubemap : public Texture {
	public:
	// Constructs a cubemap texture given a list of file paths to the
	// textures for the cubemap faces (right, left, top, bottom, back, front).
	TextureCubemap(const std::vector<std::string> &facePaths);
	void bind() const;
};