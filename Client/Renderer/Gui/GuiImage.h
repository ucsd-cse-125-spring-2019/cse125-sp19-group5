#pragma once

#include "../Renderer/Texture2d.h"
#include "GuiRect.h"

class GuiImage : public GuiRect {
	protected:
	Texture2d *texture;
	std::string image;

	public:
	void setImage(const std::string &bewImage);
	const std::string &getImage() const;

	void draw(float x, float y, float w, float h) const override;
};