#pragma once

#include <string>
#include "TextureCubemap.h"
#include "Camera.h"
#include "Shader.h"

class Skybox {
	private:
	const Camera &camera;
	TextureCubemap cubemap;
	Shader skyShader;
	GLuint VAO, VBO;

	public:
	// Loads the skybox textures from Assets/Textures/Skybox/<name>_<side>.tga
	// where <side> is one of {bk, dn, ft, lf, rt, up}.
	Skybox(const std::string &name, const Camera &camera);

	// Draws the textured cube for the skybox.
	void draw();

	~Skybox();
};
