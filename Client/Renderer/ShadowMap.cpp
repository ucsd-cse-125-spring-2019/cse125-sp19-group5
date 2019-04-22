#include "ShadowMap.h"
#include <glm/gtc/matrix_transform.hpp>

ShadowMap::ShadowMap(int width, int height)
: width(width), height(height), shadowShader("Shaders/shadow") {
	// Create a depth map texture.
	depthMap.bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		width, height,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT, nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

	// Then create a framebuffer and attach the depth map texture to it.
	FBO = 0;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthMap.getID(),
		0
	);
	glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::prePass() {
	glCullFace(GL_FRONT); // To prevent some artifacts.
	shadowShader.use();

	// Set up the viewport so stuff is drawn at the correct size.
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::setupLight(Shader &shader, const DirectionalLight &light) {
	const auto up = vec3(0.0f, 1.0f, 0.0f);
	const auto origin = vec3(0.0f);

	// TODO (bhang): adjust this to fit the scene.
	auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
	auto view = glm::lookAt(light.getDirection(), origin, up);
	shader.setUniform("toLightSpace", proj * view);
}

void ShadowMap::postPass() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glCullFace(GL_BACK);
}

void ShadowMap::bindTexture(Shader &shader) const {
	shader.setUniform("shadowMap", 31);
	glActiveTexture(GL_TEXTURE31);
	depthMap.bind(GL_TEXTURE_2D);
}

Shader &ShadowMap::getShader() {
	return shadowShader;
}

ShadowMap::~ShadowMap() {
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
	}
}