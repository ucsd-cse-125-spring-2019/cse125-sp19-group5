#include "GuiImage.h"
#include "../Assets.h"
#include "../Draw.h"

void GuiImage::setImage(const std::string &newImage) {
	image = newImage;
	texture = Assets::getTexture2d(image);
}

const std::string &GuiImage::getImage() const {
	return image;
}

void GuiImage::draw(float x, float y, float w, float h) const {
	Draw::setColor(color);
	Draw::rect(x, y, w, h, *texture);
}