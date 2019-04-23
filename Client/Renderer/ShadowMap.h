#pragma once

#include <GL/glew.h>
#include "Texture.h"
#include "Shader.h"
#include "DirectionalLight.h"

// The ShadowMap class sets up a depth map that stores depths from a light
// for use in calculating shadows.
class ShadowMap {
	private:
	int width; // Width of the depth map.
	int height; // Height of the depth map.
	int viewport[4]; // Info about viewport before rendering to depth map.
	GLuint FBO; // ID for depth map framebuffer.
	GLuint RBO; // Render buffer object to also attach depth.
	Texture depthMap; // Texture that stores depth of vertices from a light's POV.
	Shader shadowShader; // Shader for drawing to depth map.

	public:
	ShadowMap(int width = 1024, int height = 1024);

	// Does some OpenGL setup before rendering the scene for the shadow pass.
	void prePass();

	// Cleans up the set up stuff after rendering the scene for the shadow pass.
	void postPass();

	// Sets texture 31 to be the depth map - used in lighting shader.
	void bindTexture(Shader &shader) const;

	// Sets the light that will be used to create the depth map.
	void setupLight(Shader &shader, const DirectionalLight &light);

	// Returns the shadow shader.
	Shader &getShader();

	Texture &getTexture();

	~ShadowMap();
};