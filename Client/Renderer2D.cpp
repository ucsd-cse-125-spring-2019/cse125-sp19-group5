#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer2D::Renderer2D() : shader("Shaders/textured2d") {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	projection = glm::ortho(0.0f, static_cast<GLfloat>(1600), 0.0f, static_cast<GLfloat>(900)); // TODO (kpan) remove hardcode

	shader.use();
	shader.setUniform("projection", projection);
	shader.setUniform("text", 0);
}

void Renderer2D::bindShader() {
	shader.use();
}

void Renderer2D::drawTexturedRect(const Texture &texture, float x, float y, float w, float h) {
	glActiveTexture(GL_TEXTURE0);
	texture.bind();
	glBindVertexArray(VAO);

	GLfloat vertices[6][4] = {
		{ x, y + h,   0.0, 0.0 },
		{ x, y,       0.0, 1.0 },
		{ x + w, y,   1.0, 1.0 },

		{ x, y + h,     0.0, 0.0 },
		{ x + w, y,     1.0, 1.0 },
		{ x + w, y + h, 1.0, 0.0 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


Renderer2D::~Renderer2D() {
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
}
