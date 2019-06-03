#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/DirectionalLight.h"
#include "Renderer/Texture2d.h"
#include "Renderer/Skybox.h"
#include "Renderer/TextRenderer.h"
#include "Renderer/ShadowMap.h"
#include "Renderer/DrawPass.h"
#include "Sound/SoundEngine.h"
#include "Game/Gui/GuiHUD.h"
#include "ClientGameObject.h"
#include <Shared/GameState.h>
#include "Networking/Client.h"
#include <array>

class Game {
	private:
	int screenWidth = 800;
	int screenHeight = 600;

	Shader *lightShader = nullptr;
	Shader *textShader = nullptr;
	TextRenderer *textRenderer = nullptr;
	float fpsTextTimer = 0.0f;
	Text *fpsText = nullptr;
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

	std::array<ClientGameObject*, MAX_GAME_OBJS> gameObjects;

	Sound *soundtrack = nullptr;
	GuiHUD *hud = nullptr;

	int playerId;

	GameState gameState;
	Player *playerObj = nullptr;

	void updateInputs();

public:
	bool shouldExit = false;
	Game();
	~Game();

	void update(float dt);
	void draw(float dt) const;
	void drawScene(Shader &shader, DrawPass pass) const;
	void drawUI() const;

	void onGameObjectCreated(Connection *c, NetBuffer &buffer);
	void onGameObjectDeleted(Connection *c, NetBuffer &buffer);
	void onGameObjectModelSet(Connection *c, NetBuffer &buffer);
	void onGameObjectAnimSet(Connection *c, NetBuffer &buffer);
	void onGameObjectMaterialSet(Connection *c, NetBuffer &buffer);
	void onPlaySound(Connection *c, NetBuffer &buffer);

	int getScreenWidth() const;
	int getScreenHeight() const;

	void updateScreenDimensions(int width, int height);
	Camera *getCamera() const;
};

