#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/DirectionalLight.h"
#include "Renderer/Texture2d.h"
#include "Renderer/Skybox.h"

class Game {
	private:
	Shader *lightShader = nullptr;
	Model *bear = nullptr;
	Model *sphere = nullptr;
	Camera *camera = nullptr;
	DirectionalLight *sun = nullptr;
	Texture2d *grass = nullptr;
	Texture2d *white = nullptr;
	Skybox *skybox = nullptr;
	float theta = 0.0f;
	float phi = 0.0f;

	float mouseSensitivity = 1.0f;

	public:
	bool shouldExit = false;
	Game();
	~Game();

	void update(float dt);
	void draw(float dt) const;

	Camera *getCamera() const;
};

