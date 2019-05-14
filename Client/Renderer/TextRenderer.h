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

	void renderText(Shader &shader) {
		font.renderText(shader, text, widthPercent, heightPercent, scale, color);
	};
};

class TextRenderer
{
private:
	std::unordered_map<std::string, Font> fonts;
	std::vector<Text> texts;

public:
	static const std::string DEFAULT_FONT_NAME;
	static const std::string DEFAULT_FONT_FILEPATH;

	bool loadFont(const std::string &name, const std::string &filepath);
	Text* addText(const std::string &fontname, const std::string text, const GLfloat widthPercent, const GLfloat heightPercent, const GLfloat scale, const glm::vec3 color);
	void renderText();
	void renderText(
		const std::string &text,
		const std::string &font,
		float x, float y,
		const vec3 &color,
		float scale = 1.0f
	) const;
};

extern TextRenderer *gTextRenderer;
