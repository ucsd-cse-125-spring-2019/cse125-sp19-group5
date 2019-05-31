#include "Draw.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

float Draw::screenWidth = 0.0f;
float Draw::screenHeight = 0.0f;

GLuint Draw::quadVAO = 0;
GLuint Draw::quadVBO = 0;

// (x, y, z) -> (2x - 1, 2y - 1, z)
mat4 Draw::projection = mat4(
	 2.0,  0.0, 0.0, 0.0,
	 0.0,  2.0, 0.0, 0.0,
	 0.0,  0.0, 1.0, 0.0,
	-1.0, -1.0, 0.0, 1.0
);

vec4 Draw::color = vec4(1.0f);
Shader *Draw::texturedQuadShader = nullptr;
Texture2d *Draw::whiteTexture = nullptr;

void Draw::init() {
	texturedQuadShader = new Shader("Shaders/textured2d");
	whiteTexture = new Texture2d("Textures/white.png");

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	texturedQuadShader->use();
	texturedQuadShader->setUniform("projection", projection);
	texturedQuadShader->setUniform("color", color);
}

void Draw::setColor(const vec4 &newColor) {
	color = newColor;
	texturedQuadShader->setUniform("color", color);
}

void Draw::setColor(float r, float g, float b, float a) {
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	texturedQuadShader->setUniform("color", color);
}

void Draw::cleanUp() {
	delete texturedQuadShader;
}

void Draw::rect(float x, float y, float w, float h, Texture &texture) {
	glActiveTexture(GL_TEXTURE0);
	texture.bind();
	glBindVertexArray(quadVAO);

	GLfloat vertices[6][4] = {
		{ x, y + h,   0.0, 0.0 },
		{ x, y,       0.0, 1.0 },
		{ x + w, y,   1.0, 1.0 },

		{ x, y + h,     0.0, 0.0 },
		{ x + w, y,     1.0, 1.0 },
		{ x + w, y + h, 1.0, 0.0 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Draw::rect(float x, float y, float w, float h) {
	rect(x, y, w, h, *whiteTexture);
}

void Draw::setupContext() {
	texturedQuadShader->use();
}

void Draw::updateScreenDimensions(float newScrWidth, float newScrHeight) {
	screenWidth = newScrWidth;
	screenHeight = newScrHeight;
}
