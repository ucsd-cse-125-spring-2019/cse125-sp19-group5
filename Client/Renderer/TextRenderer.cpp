#include "TextRenderer.h"
#include "../Assets.h"
#include "Gui/Gui.h"
#include "Draw.h"

TextRenderer *gTextRenderer = new TextRenderer();

const std::string TextRenderer::DEFAULT_FONT_NAME = "Arial";
const std::string TextRenderer::DEFAULT_FONT_FILEPATH = "Fonts/Arial.ttf";

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
	auto shader = Assets::getShader("Shaders/text");
	shader->use();
	for (Text &text : texts) {
		text.renderText(*shader);
	}
}

void TextRenderer::renderText(
	const std::string &text,
	const std::string &font,
	float x, float y,
	const vec3 &color,
	float scale
) const {
	static auto textShader = Assets::getShader("Shaders/text");
	textShader->use();

	auto fontIt = fonts.find(font);
	if (fontIt == fonts.end()) {
		std::cerr << font << " is not a loaded font" << std::endl;
		return;
	}
	auto fontObj = fontIt->second;
	fontObj.renderText(
		*textShader,
		text,
		x, y,
		scale,
		color
	);

	Draw::setupContext();
}

vec2 TextRenderer::getTextSize(const std::string &text, const std::string &font, float scale) {
	auto it = fonts.find(font);
	if (it == fonts.end()) {
		std::cerr << font << " is not a valid font" << std::endl;
		return vec2(0.0f);
	}
	return (it->second).getSize(text, scale);
}
