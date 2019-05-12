#include "ShadowMap.h"
#include "Draw.h"
#include <glm/gtc/matrix_transform.hpp>

constexpr auto MAX_TEXTURE_UNITS = 32;
constexpr auto FRUSTUM_NUM_CORNERS = 8;

// The i_th and (i+1)_th z-values are used as the zNear and zFar for the i_th
// light projection where i = 0, 1, ..., SHADOW_NUM_CASCADES.
float ShadowMap::cascadeZCutoffs[SHADOW_NUM_CASCADES + 1] = {
	-10.0f, 20.0f, 25.0f, 75.0f, 250.0f
};

// Component wise min between 2 vectors u, v into u
inline void vecMin(vec3 &u, const vec3 &v) {
	u.x = fmin(u.x, v.x);
	u.y = fmin(u.y, v.y);
	u.z = fmin(u.z, v.z);
}

// Component wise max between 2 vectors u, v into u
inline void vecMax(vec3 &u, const vec3 &v) {
	u.x = fmax(u.x, v.x);
	u.y = fmax(u.y, v.y);
	u.z = fmax(u.z, v.z);
}

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

ShadowMap::ShadowMap(Camera *camera, int width, int height)
: width(width), height(height), shadowShader("Shaders/shadow")
, blurFilter("Shaders/Filters/gaussblur"), camera(camera) {
	// Set up the original depth map and the blurred version of it.
	for (auto &shadowMap : shadowMaps) {
		setupTexture(shadowMap);
	}
	setupTexture(blurredShadowMap);

	// Then create a framebuffer and attach the depth map texture to it.
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, shadowMaps[0].getID(),
		0
	);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		RBO
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

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
	// Set up the viewport so stuff is drawn at the correct size.
	if (i == 0) {
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, width, height);
		glDisable(GL_BLEND);
	}
	shadowShader.use();
	shadowShader.setUniform("cascade", i);
	bindLightTransforms(shadowShader);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, shadowMaps[i].getID(),
		0
	);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#include <glm/gtx/string_cast.hpp>

void ShadowMap::setupLightTransform(int i, const DirectionalLight &light) {
	// `camViewInv` transforms from view space to world space.
	auto camViewInv = glm::inverse(camera->getViewMatrix());

	// `view` transforms from world space to light space.
	constexpr auto up = vec3(0.0f, 1.0f, 0.0f);
	constexpr auto origin = vec3(0.0f);
	auto view = glm::lookAt(origin, -light.getDirection(), up);

	auto viewToLightSpace = view * camViewInv;

	// Compute camera frustum vertices in view space.
	auto halfFov = camera->getFov() * 0.5f;
	auto horz = glm::tan(glm::radians(halfFov * camera->getAspect()));
	auto vert = glm::tan(glm::radians(halfFov));
	auto zNear = cascadeZCutoffs[i], zFar = cascadeZCutoffs[i + 1];
	auto xNear = zNear * horz, xFar = zFar * horz;
	auto yNear = zNear * vert, yFar = zFar * vert;

	vec3 vertices[FRUSTUM_NUM_CORNERS] = {
		vec3( xNear,  yNear, zNear),
		vec3(-xNear,  yNear, zNear),
		vec3( xNear, -yNear, zNear),
		vec3(-xNear, -yNear, zNear),

		vec3( xFar,  yFar, zFar),
		vec3(-xFar,  yFar, zFar),
		vec3( xFar, -yFar, zFar),
		vec3(-xFar, -yFar, zFar),
	};

	// Get bounding box for the frustum.
	vec3 mins = vertices[0], maxs = vertices[0];
	for (int j = 1; j < FRUSTUM_NUM_CORNERS; j++) {
		vertices[j] = vec3(viewToLightSpace * vec4(vertices[j], 1.0f));
		vecMin(mins, vertices[j]);
		vecMax(maxs, vertices[j]);
	}

	auto proj = glm::ortho(mins.x, maxs.x, mins.y, maxs.y, mins.z, maxs.z);
	if (i == 0) {
		std::cout << mins.x << "\t" << maxs.x << "\t" << mins.y << "\t" << maxs.y << "\t" << mins.z << "\t" << maxs.z << std::endl;
	}
	toLightSpace[i] = proj * view;
}

void ShadowMap::setupLight(const DirectionalLight &light) {
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		setupLightTransform(i, light);
	}
}

void ShadowMap::bindLightTransforms(Shader &shader) const {
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		auto name = "toLightSpace[" + std::to_string(i) + "]";
		shader.setUniform(name.c_str(), toLightSpace[i]);
	}
}

void ShadowMap::bindZCutoffs(Shader &shader) const {
	auto projection = camera->getProjectionMatrix();
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		auto name = "cascadeZCutoffs[" + std::to_string(i) + "]";
		auto clipSpace = projection * vec4(0, 0, cascadeZCutoffs[i + 1], 1);
		shader.setUniform(name.c_str(), cascadeZCutoffs[i + 1]);
	}
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

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		blurFilter.setUniform(
			"blurSize",
			vec2(0.0f, 1.0f / ((float)height * amount))
		);
		Draw::rect(-1.0f, -1.0f, 2.0f, 2.0f, blurredShadowMap);
	}
}

void ShadowMap::postPass() {
	//blurDepthMap();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::bindTexture(Shader &shader) const {
	auto baseTextureUnit = GL_TEXTURE31 - SHADOW_NUM_CASCADES + 1;
	auto baseIndex = MAX_TEXTURE_UNITS - SHADOW_NUM_CASCADES;

	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		auto name = "shadowMap[" + std::to_string(i) + "]";
		shader.setUniform(name.c_str(), baseIndex + i);
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
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}
	if (blurFBO) {
		glDeleteFramebuffers(1, &blurFBO);
		blurFBO = 0;
	}
	if (RBO) {
		glDeleteRenderbuffers(1, &RBO);
	}
}