#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include "Texture.h"

class TextureCubemap : public Texture {
	public:
	TextureCubemap(const std::vector<std::string> &facePaths);
	void bind() const;
};