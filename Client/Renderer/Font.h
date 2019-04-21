#pragma once

#include <Shared/Common.h>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

struct Character {
	GLuint TextureID;	// ID handle of the glyph texture
	glm::ivec2 Size;	// Size of glyph
	glm::ivec2 Bearing;	// Offset from baseline to left/top of glyph
	GLuint Advance;		// Horizontal offset to advance to next glyph
};

class Font {
public:
	Font();
	Font(const std::string &filepath, Shader &shader);
	~Font();

	void RenderText(Shader &shader, const std::string &text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec3 &color);

private:
	std::unordered_map<GLchar, Character> characters;
	GLuint VAO;
	GLuint VBO;
};
