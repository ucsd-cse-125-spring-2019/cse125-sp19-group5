#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/DirectionalLight.h"

class Game {
	private:
	Shader *lightShader = nullptr;
	Model *bear = nullptr;
	Camera *camera = nullptr;
	DirectionalLight *sun = nullptr;
	float theta = 0.0f;

	public:
	Game();
	~Game();

	void update(float dt);
	void draw(float dt) const;

	Camera *getCamera() const;
};

