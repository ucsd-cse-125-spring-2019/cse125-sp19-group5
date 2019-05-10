#include "ShadowMap.h"
#include "Draw.h"
#include <glm/gtc/matrix_transform.hpp>

constexpr auto MAX_TEXTURE_UNITS = 32;

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
	for (auto &shadowMap : shadowMaps) {
		setupTexture(shadowMap);
	}
	setupTexture(blurredShadowMap);

	// Then create a framebuffer and attach the depth map texture to it.
	glGenFramebuffers(SHADOW_NUM_CASCADES, FBOs);
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, shadowMaps[i].getID(),
			0
		);
	}

	// Set up RBO for depth map so color + depth works correctly.
	glGenRenderbuffers(SHADOW_NUM_CASCADES, RBOs);
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBOs[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			RBOs[i]
		);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	// Then, set up FBO so we can blur the depth map.
	blurFBO = 0;
	glGenFramebuffers(1, &blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, blurredShadowMap.getID(),
		0
	);

	// Finally, unbind things so they don't get overwritten.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::prePass(int i) {
	shadowShader.use();

	// Set up the viewport so stuff is drawn at the correct size.
	if (i == 0) {
		glGetIntegerv(GL_VIEWPORT, viewport);
	}
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);

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

	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		// Set the texture that will be blurred.
		blurFilter.setUniform(
			"blurSize",
			vec2(1.0f / ((float)width * amount), 0.0f)
		);
		Draw::rect(-1.0f, -1.0f, 2.0f, 2.0f, shadowMaps[i]);

		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		blurFilter.setUniform(
			"blurSize",
			vec2(0.0f, 1.0f / ((float)height * amount))
		);
		Draw::rect(-1.0f, -1.0f, 2.0f, 2.0f, blurredShadowMap);
	}
}

void ShadowMap::postPass() {
	blurDepthMap();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::bindTexture(Shader &shader) const {
	auto baseTextureUnit = GL_TEXTURE31 - SHADOW_NUM_CASCADES + 1;
	auto baseIndex = MAX_TEXTURE_UNITS - SHADOW_NUM_CASCADES;

	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		shader.setUniform("shadowMap", baseIndex + i);
		glActiveTexture(baseTextureUnit + i);
		shadowMaps[i].bind(GL_TEXTURE_2D);
	}
	glActiveTexture(GL_TEXTURE0);
}

Shader &ShadowMap::getShader() {
	return shadowShader;
}

Texture &ShadowMap::getTexture(int i) {
	return shadowMaps[i];
}

ShadowMap::~ShadowMap() {
	if (FBOs[0]) {
		glDeleteFramebuffers(SHADOW_NUM_CASCADES, FBOs);
	}
	if (blurFBO) {
		glDeleteFramebuffers(1, &blurFBO);
		blurFBO = 0;
	}
	if (RBOs[0]) {
		glDeleteRenderbuffers(SHADOW_NUM_CASCADES, RBOs);
	}
}