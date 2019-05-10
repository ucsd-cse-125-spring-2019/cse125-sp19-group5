#pragma once

#include <GL/glew.h>
#include "Texture.h"
#include "Shader.h"
#include "DirectionalLight.h"
#include "Camera.h"

constexpr auto SHADOW_NUM_CASCADES = 1;

// The ShadowMap class sets up a depth map that stores depths from a light
// for use in calculating shadows.
class ShadowMap {
	private:
	int width; // Width of the depth map.
	int height; // Height of the depth map.
	int viewport[4]; // Info about viewport before rendering to depth map.
	GLuint FBOs[SHADOW_NUM_CASCADES] = { 0 };
	GLuint RBOs[SHADOW_NUM_CASCADES] = { 0 };
	Texture shadowMaps[SHADOW_NUM_CASCADES];
	GLuint blurFBO = 0;
	Texture blurredShadowMap; // depthMap + Gaussian Blur
	Shader shadowShader; // Shader for drawing to depth map.
	Shader blurFilter; // Shader that does Gaussian blur.
	Camera *camera = nullptr;
	mat4 toLightSpace[SHADOW_NUM_CASCADES];

	void setupTexture(Texture &texture);
	void blurDepthMap(float amount = 0.5f);

	// Set up the light space transformation for the i_th cascade.
	void setupLightTransform(int i, const DirectionalLight &light);

	public:
	static float cascadeZCutoffs[SHADOW_NUM_CASCADES + 1];

	ShadowMap(Camera *camera, int width = 1024, int height = 1024);

	// Does some OpenGL setup before rendering the scene for the shadow pass.
	void prePass(int i);

	// Cleans up the set up stuff after rendering the scene for the shadow pass.
	void postPass();

	// Sets texture 31 to be the depth map - used in lighting shader.
	void bindTexture(Shader &shader) const;

	// Sets up the light space transform uniforms for the given shader.
	void bindLightTransforms(Shader &shader) const;

	// Sets the light that will be used to create the depth map.
	void setupLight(const DirectionalLight &light);

	// Returns the shadow shader.
	Shader &getShader();

	Texture &getTexture(int i);

	~ShadowMap();
};