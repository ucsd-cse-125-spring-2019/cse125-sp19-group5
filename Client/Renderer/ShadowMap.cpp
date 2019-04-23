#include "ShadowMap.h"
#include <glm/gtc/matrix_transform.hpp>

void checkerr() {
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << glewGetErrorString(err) << std::endl;
	} else {
		std::cerr << "OK!" << std::endl;
	}
}
ShadowMap::ShadowMap(int width, int height)
: width(width), height(height), shadowShader("Shaders/shadow") {
	// Create a depth map texture.
	depthMap.bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_R32F,
		width, height,
		0,
		GL_RGBA,
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
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, depthMap.getID(),
		0
	);

	RBO = 0;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::prePass() {
	//glCullFace(GL_FRONT); // To prevent some artifacts.
	shadowShader.use();

	// Set up the viewport so stuff is drawn at the correct size.
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
}

void ShadowMap::setupLight(Shader &shader, const DirectionalLight &light) {
	const auto up = vec3(0.0f, 1.0f, 0.0f);
	const auto origin = vec3(0.0f);
	auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 100.0f);
	auto view = glm::lookAt(light.getDirection(), origin, up);
	shader.setUniform("toLightSpace", proj * view);
}

void ShadowMap::postPass() {
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::bindTexture(Shader &shader) const {
	shader.setUniform("shadowMap", 31);
	glActiveTexture(GL_TEXTURE31);
	depthMap.bind(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
}

Shader &ShadowMap::getShader() {
	return shadowShader;
}

Texture &ShadowMap::getTexture() {
	return depthMap;
}

ShadowMap::~ShadowMap() {
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
	}
	if (RBO) {
		glDeleteRenderbuffers(1, &RBO);
		RBO = 0;
	}
}