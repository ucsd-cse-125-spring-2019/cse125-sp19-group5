#pragma once

#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include <Shared/Common.h>

class Renderer2D {
	private:
	GLuint VAO, VBO;
	mat4 projection;
	Shader shader;

	public:
	Renderer2D();
	void bindShader();
	void drawTexturedRect(const Texture &texture, float x, float y, float w, float h);
	~Renderer2D();
};

