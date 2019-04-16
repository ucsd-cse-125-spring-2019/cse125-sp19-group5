#include "Skybox.h"
#include <Shared/Common.h>

const float SKYBOX_VERTICES[] = {
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	// Up
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

	// Down
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

Skybox::Skybox(const std::string &name, const Camera &camera) : cubemap({
	name + "_rt.tga", name + "_lf.tga",
	name + "_up.tga", name + "_dn.tga",
	name + "_bk.tga", name + "_ft.tga",
}), camera(camera), skyShader("Shaders/sky") {
	VAO = 0;
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(SKYBOX_VERTICES),
		SKYBOX_VERTICES,
		GL_STATIC_DRAW
	);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	skyShader.use();
	skyShader.setUniform("cubemap", 0);
}

void Skybox::draw() {
	glDepthFunc(GL_LEQUAL);

	skyShader.use();

	auto mvp = camera.getProjectionMatrix()
		* mat4(mat3(camera.getViewMatrix()));

	skyShader.setUniform("mvp", mvp);
	skyShader.setUniform("cubemap", 0);

	glBindVertexArray(VAO);
	cubemap.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

Skybox::~Skybox() {
	if (VBO) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}
