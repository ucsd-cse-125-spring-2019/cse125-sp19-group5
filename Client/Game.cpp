#include <GL/glew.h>
#include "Game.h"
#include "Game/Gui/GuiConnectMenu.h"
#include "Game/Gui/GuiTeamMenu.h"
#include "Game/Gui/GuiGameText.h"
#include <Shared/Common.h>
#include <Shared/CommonStructs.h>
#include <Shared/ConfigSettings.h>
#include <glm/gtx/transform.hpp>
#include "Input.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Networking/Client.h"
#include "Renderer/Draw.h"
#include <Shared/CommonStructs.h>
#include "Renderer/Material.h"
#include "Renderer/ParticleSystem.h"
#include "Assets.h"
#include "Game/ParticleEmitters.h"
#include "Game/Gui/GuiScoreboard.h"

// Define this if you just want to go right to the game.
// #define _DEBUG_SP

GuiGameText *gameText = nullptr;

static void setGameText(Connection *c, NetBuffer &buffer) {
	auto newText = buffer.read<string>();
	if (newText == "") {
		if (gameText) {
			gameText->remove();
			gameText = nullptr;
		}
		return;
	}
	if (gameText) {
		gameText->setText(newText);
		return;
	}
	gameText = Gui::create<GuiGameText>();
	gameText->setText(newText);

}

void Game::onGameObjectCreated(Connection *c, NetBuffer &buffer) {
	auto gameObjectType = buffer.read<GAMEOBJECT_TYPES>();
	std::unique_ptr<GameObject> obj = nullptr;
	switch (gameObjectType) {
		case GAMEOBJECT_TYPES::PLAYER_TYPE:
			obj = std::make_unique<Player>(-1);
			break;
		default:
			obj = std::make_unique<GameObject>();
			break;
	}

	obj->deserialize(buffer);

	auto clientObj = new ClientGameObject(std::move(obj));
	// Do not use `obj` after this, ownership transfered to clientObj.

	auto id = clientObj->getGameObject()->getId();
	gameObjects[id] = clientObj;
	gameState.gameObjects[id] = clientObj->getGameObject();

	std::cout << "Created object " << id << std::endl;
	if (id == playerId) {
		playerObj = static_cast<Player*>(clientObj->getGameObject());
	}
}

void Game::onGameObjectDeleted(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	if (gameObjects[id]) {
		delete gameObjects[id];
	}
	gameObjects[id] = nullptr;
	gameState.gameObjects[id] = nullptr;
}

void Game::onGameObjectModelSet(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	auto model = buffer.read<std::string>();
	auto gameObject = gameObjects[id];

	if (gameObject) {
		gameObject->setModel(model);
	}
}

void Game::onGameObjectAnimSet(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	auto animationId = buffer.read<int>();
	auto reset = buffer.read<bool>();
	auto gameObject = gameObjects[id];
	if (gameObject) {
		gameObject->setAnimation(animationId, reset);
	}
}

void Game::onGameObjectMaterialSet(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	auto newMaterial = buffer.read<std::string>();
	auto gameObject = gameObjects[id];
	if (gameObject) {
		gameObject->setMaterial(newMaterial);
	}
}

void Game::onPlaySound(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	auto sound = buffer.read<string>();
	auto volume = buffer.read<float>();
	auto loop = buffer.read<bool>();
	auto gameObject = gameObjects[id];
	if (!gameObject) { return; }
	if (volume > 0.0f) {
		gameObject->playSound(sound, volume, loop);
	} else {
		gameObject->stopSound(sound);
	}
}

int Game::getScreenWidth() const {
	return screenWidth;
}

int Game::getScreenHeight() const {
	return screenHeight;
}

Game::Game() : gameObjects({ nullptr }) {
	Draw::init();
	ParticleEmitters::init(&gameState);
	auto connectMenu = Gui::create<GuiConnectMenu>();
	connectMenu->setGame(this);
	ConnectMenuBackground = gSound->loadFlatSound("Sounds/ConnectMenuMusic.wav", 0.3f);
	ConnectMenuBackground->play(true);
	int port = 1234;
	ConfigSettings::get().getValue("Port", port);

	gSound->setMasterVolume(1.0f);
	
	lightShader = new Shader("Shaders/light");
	camera = new Camera(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f), 70, 1.0f);
	shadowMap = new ShadowMap(camera);
	sun = new DirectionalLight(0);
	sun->setDirection(vec3(0.009395, -0.800647, -0.713446));
	sun->setAmbient(vec3(0.04f, 0.05f, 0.13f));
	sun->setColor(vec3(0.8f, 0.7f, 0.55f));

	shadowMap = new ShadowMap(camera);

	skybox = new Skybox("Textures/Skybox/cloudtop", *camera);

	ConfigSettings::get().getValue("MouseSensitivity", mouseSensitivity);

	gTextRenderer->loadFont(
		TextRenderer::DEFAULT_FONT_NAME,
		{ 48, TextRenderer::DEFAULT_FONT_FILEPATH }
	);
	fpsText = gTextRenderer->addText(TextRenderer::DEFAULT_FONT_NAME, "fps", 0.02f, 0.02f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));

	// Handle game object creation and deletion.
	Network::on(
		NetMessage::GAME_OBJ_CREATE,
		boost::bind(&Game::onGameObjectCreated, this, _1, _2)
	);
	Network::on(
		NetMessage::GAME_OBJ_DELETE,
		boost::bind(&Game::onGameObjectDeleted, this, _1, _2)
	);

	Network::on(
		NetMessage::GAME_OBJ_MODEL,
		boost::bind(&Game::onGameObjectModelSet, this, _1, _2)
	);
	Network::on(
		NetMessage::GAME_OBJ_ANIM,
		boost::bind(&Game::onGameObjectAnimSet, this, _1, _2)
	);
	Network::on(
		NetMessage::GAME_OBJ_MAT,
		boost::bind(&Game::onGameObjectMaterialSet, this, _1, _2)
	);

	// Receive connection id / player id from server
	Network::on(NetMessage::CONNECTION_ID, [this] (Connection *c, NetBuffer &buffer) {
		playerId = buffer.read<int>();
		cout << "I am Player " << playerId << "." << endl;
		auto size = buffer.read<size_t>();
		int p;
		std::string n;
		for (size_t i = 0; i < size; i++) {
			p = buffer.read<int>();
			n = buffer.read<std::string>();
			id_name[p] = n;
		}
		guiTeamMenu->setPlayerId(playerId);
		/*MainMenuBackground = gSound->loadFlatSound("Sounds/MainMenuMusic.wav", 0.1f);
		ConnectMenuBackground->stop();
		MainMenuBackground->play(true);
		GuiTeamMenu *teamMenu = Gui::create<GuiTeamMenu>();
		teamMenu->setPlayerId(playerId);
		teamMenu->setGame(this);*/
	});

	Network::on(NetMessage::NAME, [this](Connection*c, NetBuffer &buffer) {
		int id = buffer.read<int>();
		std::string name = buffer.read<std::string>();
		id_name[id] = name;
		/*if (id == playerId) {
			MainMenuBackground = gSound->loadFlatSound("Sounds/MainMenuMusic.wav", 0.1f);
			ConnectMenuBackground->stop();
			MainMenuBackground->play(true);
			GuiTeamMenu *teamMenu = Gui::create<GuiTeamMenu>();
			teamMenu->setPlayerId(playerId);
			teamMenu->setGame(this);
		}*/
	});

	Network::on(NetMessage::RESET, [&](Connection*c, NetBuffer &buffer) {
		teamMenu();
		guiTeamMenu->setPlayerTeam(player_team);
		guiTeamMenu->setPlayerId(playerId);
		guiTeamMenu->resetGui();
	});

	Network::on(NetMessage::GAME_STATE_UPDATE, [&](Connection *c, NetBuffer &buffer) {
		gameState.deserialize(buffer);
	});

	Network::on(NetMessage::PARTICLES, ParticleEmitters::onUpdate);
	Network::on(NetMessage::PARTICLES_DELETE, ParticleEmitters::onDelete);
	Network::on(
		NetMessage::SOUND,
		boost::bind(&Game::onPlaySound, this, _1, _2)
	);

	Network::on(NetMessage::GAME_TEXT, setGameText);
	Network::on(
		NetMessage::SCOREBOARD_SHOW,
		[&](Connection *c, NetBuffer &buffer) {
			if (hud) {
				hud->remove();
				hud = nullptr;
			}
			Gui::create<GuiScoreboard>();
		}
	);
	Network::on(
		NetMessage::HUD_VISIBLE,
		[&](Connection *c, NetBuffer &buffer) {
			auto isVisible = buffer.read<bool>();
			if (isVisible && !hud) {
				hud = Gui::create<GuiHUD>();
				if (MainMenuBackground) {
					MainMenuBackground->setVolume(0.05f);
				}
			} else if (!isVisible && hud) {
				hud->remove();
				hud = nullptr;
			}
		}
	);
}

Game::~Game() {
	delete lightShader;
	delete textShader;
	delete textRenderer;
	delete camera;
	delete sun;
	delete shadowMap;
	delete soundtrack;

	for (auto gameObject : gameObjects) {
		if (gameObject) {
			delete gameObject;
		}
	}

	Gui::cleanUp();
	Draw::cleanUp();
	ParticleEmitters::cleanUp();

	delete gSound;
}

Camera *Game::getCamera() const {
	return camera;
}

void Game::updateScreenDimensions(int width, int height) {
	screenWidth = width;
	screenHeight = height;
}

void Game::updateInputs() {
	// The camera can look up/down less than 90 degrees.
	constexpr auto MAX_PITCH_OFFSET = glm::pi<float>() / 2.0f - 0.00001f;

	float mouseMoveScale = mouseSensitivity * 0.001f;
	theta += (float)Input::getMouseDeltaX() * mouseMoveScale;
	phi += (float)Input::getMouseDeltaY() * mouseMoveScale;

	phi = glm::clamp(phi, -MAX_PITCH_OFFSET, MAX_PITCH_OFFSET);
	camera->setEyeAngles(vec3(-phi, theta, 0));

	int keyInputs = 0;
	vec3 direction(0.0f);
	if (Input::isKeyDown(GLFW_KEY_W)) {
		//direction += camera->getForward();
		keyInputs += FORWARD;
	}
	if (Input::isKeyDown(GLFW_KEY_S)) {
		//direction -= camera->getForward();
		keyInputs += BACKWARD;
	}
	if (Input::isKeyDown(GLFW_KEY_A)) {
		//direction -= camera->getRight();
		keyInputs += LEFT;
	}
	if (Input::isKeyDown(GLFW_KEY_D)) {
		//direction += camera->getRight();
		keyInputs += RIGHT;
	}
	if (Input::isKeyDown(GLFW_KEY_SPACE)) {
		keyInputs += JUMP;
	}
	if (Input::isMouseClicked(GLFW_MOUSE_BUTTON_LEFT)) {
		keyInputs += SWING;
	}
	if (Input::isMouseClicked(GLFW_MOUSE_BUTTON_RIGHT)) {
		keyInputs += SHOOT;
	}
	if (Input::isKeyDown(GLFW_KEY_F)) {
		keyInputs += WALL;
	}
	if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
		shouldExit = true;
	}

#ifdef _DEBUG_MAP_EDITOR
	if (Input::isKeyDown(GLFW_KEY_F1)) {
		NetBuffer buf(NetMessage::DEBUG_MAP);
		Network::send(buf);
		Input::setMouseVisible(false);
	}
#endif

	// Sending player input 
	NetBuffer buffer(NetMessage::PLAYER_INPUT);
	buffer.write(keyInputs);
	buffer.write(camera->getForward());
	Network::send(buffer);
}

void Game::update(float dt) {
	gSound->update(camera->getPosition(), vec3(0.0f, 0.0f, 0.0f), camera->getForward());

	fpsTextTimer += dt;
	int fps = (int) (1.0f / dt);
	if (fpsTextTimer > 0.3f)
	{
		fpsTextTimer = 0.0f;
		fpsText->text = "fps: " + std::to_string(fps);
	}

	updateInputs();

	const auto curTime = (float)glfwGetTime();
	for (auto gameObject : gameObjects) {
		if (!gameObject) { continue; }
		gameObject->update(curTime);
	}

	if (playerObj) {
		auto offset = camera->getForward() * -10.0f + vec3(0, 2, 0);
		camera->setPosition(playerObj->getPosition() + offset);
	}

	ParticleEmitters::update(dt, camera);

	if (hud) {
		hud->setTime(gameState.timeLeft);
		hud->setLeftTeamScore(std::get<0>(gameState.score));
		hud->setRightTeamScore(std::get<1>(gameState.score));
	}
}

void Game::drawScene(Shader &shader, DrawPass pass) const {
	for (auto gameObject : gameObjects) {
		if (!gameObject) { continue; }
		if (
			pass == DrawPass::SHADOW &&
			!gameObject->getGameObject()->shouldCastShadow()
		) {
			continue;
		}
		gameObject->draw(shader, camera, pass);
	}
}

void Game::drawUI() const {
	Gui::draw();

	// Draw::setColor(vec4(1.0f));
	// Draw::rect(0.0f, 0.0f, 0.25f, 0.25f, shadowMap->getTexture(2));

	gTextRenderer->renderText();
}

void Game::draw(float dt) const {
	// Shadow mapping render pass
	shadowMap->setupLight(*sun);
	for (int i = 0; i < SHADOW_NUM_CASCADES; i++) {
		shadowMap->prePass(i);
		drawScene(shadowMap->getShader(), DrawPass::SHADOW);
		shadowMap->postPass(i);
	}

	// Normal 3D render pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightShader->use();
	lightShader->setUniform("eyePos", camera->getPosition());
	lightShader->setUniform("directionalLightNum", 1);
	shadowMap->bindLightTransforms(*lightShader);
	shadowMap->bindTexture(*lightShader);
	shadowMap->bindZCutoffs(*lightShader);

	sun->bind(*lightShader);
	drawScene(*lightShader, DrawPass::LIGHTING);
	skybox->draw();
	ParticleEmitters::draw(camera);

	glDisable(GL_DEPTH_TEST);
	Draw::setupContext();
	drawUI();
	glEnable(GL_DEPTH_TEST);
}

void Game::setPlayerTeam(unordered_map<int, int> &p_t) {
	player_team = p_t;
}

unordered_map<int, std::string> Game::getIdName() {
	return id_name;
}

void Game::teamMenu() {
	MainMenuBackground = gSound->loadFlatSound("Sounds/MainMenuMusic.wav", 0.1f);
	ConnectMenuBackground->stop();
	MainMenuBackground->play(true);
	guiTeamMenu = Gui::create<GuiTeamMenu>();
	guiTeamMenu->setGame(this);
}