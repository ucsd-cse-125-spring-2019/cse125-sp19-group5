#include "TextRenderer.h"

TextRenderer::TextRenderer(Shader &shader)
	: shader(shader) {
	loadFont(DEFAULT_FONT_NAME, DEFAULT_FONT_FILEPATH);
}

TextRenderer::~TextRenderer() {

}

void TextRenderer::updateScreenDimensions(int width, int height) {
	screenWidth = width;
	screenHeight = height;
}

bool TextRenderer::loadFont(const std::string &name, const std::string &filepath) {
	try {
		Font font = Font(filepath);
		fonts.insert(std::pair<std::string, Font>(name, font));
		return true;
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}

Text* TextRenderer::addText(const std::string &fontname, const std::string text, const GLfloat widthPercent, const GLfloat heightPercent, const GLfloat scale, const glm::vec3 color) {
	Font font = fonts[DEFAULT_FONT_NAME];
	if (fonts.find(fontname) != fonts.end()) font = fonts[fontname];

	Text textObj = {
		font,
		text,
		widthPercent,
		heightPercent,
		scale,
		color
	};
	texts.push_back(textObj);
	return &texts.back();
}

void TextRenderer::renderText() {
	shader.use();
	for (Text &text : texts) {
		text.renderText(shader, screenHeight, screenHeight);
	}
}
