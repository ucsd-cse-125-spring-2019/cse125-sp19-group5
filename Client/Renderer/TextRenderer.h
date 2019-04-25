#pragma once

#include <Shared/Common.h>
#include <unordered_set>
#include <unordered_map>

#include "Shader.h"
#include "Font.h"

struct Text {
	Font font;
	std::string text;
	GLfloat widthPercent;
	GLfloat heightPercent;
	GLfloat scale;
	vec3 color;

	void renderText(Shader &shader, int screenWidth, int screenHeight) {
		font.renderText(shader, screenWidth, screenHeight, text, widthPercent * screenWidth, heightPercent * screenHeight, scale, color);
	};
};

class TextRenderer
{
private:
	int screenWidth = 800;
	int screenHeight = 600;
	Shader &shader;
	std::unordered_map<std::string, Font> fonts;
	std::vector<Text> texts;

public:
	const std::string DEFAULT_FONT_NAME = "Arial";
	const std::string DEFAULT_FONT_FILEPATH = "Fonts/Arial.ttf";

	TextRenderer(Shader &shader);
	~TextRenderer();

	void updateScreenDimensions(int width, int height);
	bool loadFont(const std::string &name, const std::string &filepath);
	Text* addText(const std::string &fontname, const std::string text, const GLfloat widthPercent, const GLfloat heightPercent, const GLfloat scale, const glm::vec3 color);
	void renderText();
};
