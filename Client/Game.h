#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/DirectionalLight.h"
#include "Renderer/Texture2d.h"
#include "Renderer/Skybox.h"
#include "Renderer/TextRenderer.h"
#include "Renderer/ShadowMap.h"

class Game {
	private:
	int screenWidth = 800;
	int screenHeight = 600;

	Shader *lightShader = nullptr;
	Shader *textShader = nullptr;
	TextRenderer *textRenderer = nullptr;
	float testTextChange = 0.0f; // Remove later
	Text *testText = nullptr; // Remove later
	Model *bear = nullptr;
	Model *sphere = nullptr;
	Camera *camera = nullptr;
	DirectionalLight *sun = nullptr;
	Texture2d *grass = nullptr;
	Texture2d *white = nullptr;
	Skybox *skybox = nullptr;
	ShadowMap *shadowMap = nullptr;
	float theta = 0.0f;
	float phi = 0.0f;
	float ballX = 0.0f;
	float mouseSensitivity = 1.0f;

public:
	bool shouldExit = false;
	Game();
	~Game();

	void update(float dt);
	void draw(float dt) const;
	void drawScene(Shader &shader) const;
	void drawUI() const;

	void updateScreenDimensions(int width, int height);
	Camera *getCamera() const;
};

