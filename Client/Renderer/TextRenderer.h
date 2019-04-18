#pragma once

#include <Shared/Common.h>
#include <unordered_set>
#include <unordered_map>

#include "Shader.h"
#include "Font.h"

struct Text {
	Font font;
	std::string text;
	GLfloat x;
	GLfloat y;
	GLfloat scale;
	vec3 color;

	void renderText(Shader &shader) {
		font.RenderText(shader, text, x, y, scale, color);
	};
};

class TextRenderer
{
private:
	Shader shader;
	std::unordered_map<std::string, Font> fonts;
	std::vector<Text> texts;

public:
	const std::string DEFAULT_FONT_NAME = "Arial";
	const std::string DEFAULT_FONT_FILEPATH = "Fonts/Arial.ttf";

	TextRenderer(Shader &shader);
	~TextRenderer();

	bool loadFont(const std::string &name, const std::string &filepath);
	Text* addText(const std::string &fontname, const std::string text, const GLfloat x, const GLfloat y, const GLfloat scale, const glm::vec3 color);
	void renderText();
};
