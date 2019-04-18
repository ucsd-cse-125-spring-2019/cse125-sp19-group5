#include "TextRenderer.h"

TextRenderer::TextRenderer(Shader &shader)
	: shader(shader) {
	loadFont(DEFAULT_FONT_NAME, DEFAULT_FONT_FILEPATH);
}

bool TextRenderer::loadFont(const std::string &name, const std::string &filepath) {
	try {
		Font font = Font(filepath, shader);
		fonts.insert(std::pair<std::string, Font>(name, font));
		return true;
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}

Text* TextRenderer::addText(const std::string &fontname, const std::string text, const GLfloat x, const GLfloat y, const GLfloat scale, const glm::vec3 color) {
	Font font = fonts[DEFAULT_FONT_NAME];
	if (fonts.find(fontname) != fonts.end()) font = fonts[fontname];

	Text textObj = {
		font,
		text,
		x,
		y,
		scale,
		color
	};
	texts.push_back(textObj);
	return &texts.back();
}

void TextRenderer::renderText() {
	for (Text text : texts) {
		text.renderText(shader);
	}
}
