#include "Game.h"
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
#include "Game/Gui/GuiConnectMenu.h"

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

int Game::getScreenWidth() const {
	return screenWidth;
}

int Game::getScreenHeight() const {
	return screenHeight;
}

Game::Game(): gameObjects(1024, nullptr) {
	Draw::init();

	// TODO (bhang): Integrate this with connecting.
	// Gui::create<GuiConnectMenu>();
	
	Input::setMouseVisible(false);

	lightShader = new Shader("Shaders/light");
	camera = new Camera(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f), 70, 1.0f);
	shadowMap = new ShadowMap(camera);
	sun = new DirectionalLight(0);
	sun->setDirection(vec3(0.009395, -0.500647, -0.713446));
	sun->setAmbient(vec3(0.04f, 0.05f, 0.13f));
	sun->setColor(vec3(0.8f, 0.7f, 0.55f));

	shadowMap = new ShadowMap(camera);

	skybox = new Skybox("Textures/Skybox/cloudtop", *camera);

	ConfigSettings::get().getValue("MouseSensitivity", mouseSensitivity);

	gTextRenderer->loadFont(
		TextRenderer::DEFAULT_FONT_NAME,
		TextRenderer::DEFAULT_FONT_FILEPATH
	);
	fpsText = gTextRenderer->addText(TextRenderer::DEFAULT_FONT_NAME, "fps", 0.02f, 0.02f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));

	soundEngine = new SoundEngine();
	soundEngine->setMasterVolume(1.0f);
	soundtrack = soundEngine->loadFlatSound("Sounds/minecraft_wet_hands.wav", 0.1f);
	soundtrack->play(true);
	spatialTest1 = soundEngine->loadSpatialSound("Sounds/minecraft_sheep.ogg", 1.0f);
	spatialTest1->play(false);
	spatialTest2 = soundEngine->loadSpatialSound("Sounds/minecraft_chicken_ambient.ogg", 1.0f);
	spatialTest2->play(false);

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
	});

	Network::on(NetMessage::GAME_STATE_UPDATE, [&](Connection *c, NetBuffer &buffer) {
		gameState.deserialize(buffer);
	});
}

Game::~Game() {
	delete lightShader;
	delete textShader;
	delete textRenderer;
	delete camera;
	delete sun;
	delete shadowMap;
	delete soundtrack;
	delete spatialTest1;
	delete spatialTest2;

	for (auto gameObject : gameObjects) {
		if (gameObject) {
			delete gameObject;
		}
	}

	Gui::cleanUp();
	Draw::cleanUp();
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
	if (Input::isKeyDown(GLFW_KEY_E)) {
		keyInputs += SWING;
	}

	if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
		shouldExit = true;
	}

	// Sending player input 
	NetBuffer buffer(NetMessage::PLAYER_INPUT);
	buffer.write(keyInputs);
	buffer.write(camera->getForward());
	Network::send(buffer);
}

void Game::update(float dt) {
	spatialTest1->setPosition(camera->getPosition() + vec3(10.0f, 0.0f, 0.0f));
	spatialTest2->setPosition(camera->getPosition() + vec3(10.0f, 0.0f, 0.0f));
	soundEngine->update(camera->getPosition(), vec3(0.0f, 0.0f, 0.0f), camera->getForward());

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
		gameObject->updateAnimation(curTime);
	}

	if (playerObj) {
		auto offset = camera->getForward() * -10.0f + vec3(0, 2, 0);
		camera->setPosition(playerObj->getPosition() + offset);
	}
}

void Game::drawScene(Shader &shader, DrawPass pass) const {
	for (auto gameObject : gameObjects) {
		if (!gameObject) { continue; }
		gameObject->draw(shader, camera, pass);
	}
}

void Game::drawUI() const {
	Gui::draw();

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

	glDisable(GL_DEPTH_TEST);
	Draw::setupContext();
	drawUI();
	glEnable(GL_DEPTH_TEST);
}
