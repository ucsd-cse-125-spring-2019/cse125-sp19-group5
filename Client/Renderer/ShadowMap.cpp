#include "ShadowMap.h"
#include "Draw.h"
#include <glm/gtc/matrix_transform.hpp>

void ShadowMap::setupTexture(Texture &texture) {
	static float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	texture.bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RG32F,
		width, height,
		0,
		GL_RGBA,
		GL_FLOAT, nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);
}

ShadowMap::ShadowMap(int width, int height)
: width(width), height(height), shadowShader("Shaders/shadow")
, blurFilter("Shaders/Filters/gaussblur") {
	// Set up the original depth map and the blurred version of it.
	setupTexture(depthMap);
	setupTexture(blurredDepthMap);

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

	// Set up RBO for depth map so color + depth works correctly.
	RBO = 0;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Then, set up FBO so we can blur the depth map.
	blurFBO = 0;
	glGenFramebuffers(1, &blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, blurredDepthMap.getID(),
		0
	);


	// Finally, unbind things so they don't get overwritten.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::prePass() {
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

void ShadowMap::blurDepthMap(float amount) {
	Draw::setColor();
	blurFilter.use();

	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	// Set the texture that will be blurred.
	blurFilter.setUniform("blurSize", vec2(1.0f / ((float)width * amount), 0.0f));
	Draw::rect(-1.0f, -1.0f, 2.0f, 2.0f, depthMap);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blurFilter.setUniform("blurSize", vec2(0.0f, 1.0f / ((float)height * amount)));
	Draw::rect(-1.0f, -1.0f, 2.0f, 2.0f, blurredDepthMap);
}

void ShadowMap::postPass() {
	blurDepthMap();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glEnable(GL_BLEND);
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