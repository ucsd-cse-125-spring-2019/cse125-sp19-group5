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

Material *testMat = nullptr;

void Game::onGameObjectCreated(Connection *c, NetBuffer &buffer) {
	auto gameObjectType = buffer.read<GAMEOBJECT_TYPES>();
	std::unique_ptr<GameObject> obj = nullptr;
	switch (gameObjectType) {
		case GAMEOBJECT_TYPES::PLAYER_TYPE:
			obj = std::make_unique<Player>(-1);
			break;
		default:
			std::cerr << "Unknown game object type (" << gameObjectType << ")"
				<< std::endl;
			return;
	}

	obj->deserialize(buffer);

	auto clientObj = new ClientGameObject(std::move(obj));
	// Do not use `obj` after this, ownership transfered to clientObj.
	clientObj->setModel("Models/sphere.obj");
	gameObjects[clientObj->getGameObject()->getId()] = clientObj;
}

void Game::onGameObjectDeleted(Connection *c, NetBuffer &buffer) {
	auto id = buffer.read<int>();
	if (gameObjects[id]) {
		delete gameObjects[id];
		gameObjects[id] = nullptr;
	}
}

Game::Game(): gameObjects(1024, nullptr) {
	testMat = new Material("Materials/brick.json");
	Draw::init();

	shadowMap = new ShadowMap();
	lightShader = new Shader("Shaders/light");
	textShader = new Shader("Shaders/text");
	camera = new Camera(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f), 70, 1.0f);
	sun = new DirectionalLight(0);
	sun->setDirection(vec3(0.009395, -0.200647, -0.713446));
	sun->setAmbient(vec3(0.04f, 0.05f, 0.13f));
	sun->setColor(vec3(0.8f, 0.7f, 0.55f));

	skybox = new Skybox("Textures/Skybox/cloudtop", *camera);

	white = new Texture2d("Textures/white.png");
	grass = new Texture2d("Textures/grass.png");

	Input::setMouseVisible(false);

	ConfigSettings::get().getValue("MouseSensitivity", mouseSensitivity);

	textRenderer = new TextRenderer(*textShader);
	fpsText = textRenderer->addText(textRenderer->DEFAULT_FONT_NAME, "fps", 0.02f, 0.02f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));

	soundEngine = new SoundEngine();
	soundEngine->setMasterVolume(1.0f);
	soundtrack = soundEngine->loadFlatSound("Sounds/minecraft_wet_hands.wav", 0.5f);
	soundtrack->play(true);
	spatialTest1 = soundEngine->loadSpatialSound("Sounds/minecraft_sheep.ogg", 1.0f);
	spatialTest1->play(true);
	spatialTest2 = soundEngine->loadSpatialSound("Sounds/minecraft_chicken_ambient.ogg", 1.0f);
	spatialTest2->play(true);

	// Handle game object creation and deletion.
	Network::on(
		NetMessage::GAME_OBJ_CREATE,
		boost::bind(&Game::onGameObjectCreated, this, _1, _2)
	);
	Network::on(
		NetMessage::GAME_OBJ_DELETE,
		boost::bind(&Game::onGameObjectDeleted, this, _1, _2)
	);

	// Receive connection id / player id from server
	Network::on(NetMessage::CONNECTION_ID, [this] (Connection *c, NetBuffer &buffer) {
		playerId = buffer.read<int>();
		cout << "I am Player " << playerId << "." << endl;
	});

	GameStateNet *gsn = new GameStateNet();

	Network::on(NetMessage::GAME_STATE_UPDATE, [this,gsn](Connection *c, NetBuffer &buffer) {
		gsn->deserialize(buffer);
		/*TODO: graphics update based on the game state*/
		for (auto gObj : gsn->gameObjects) {
			if (!gObj) { continue; }
			auto clientObj = gameObjects[gObj->getId()];

			if (clientObj) {
				clientObj->getGameObject()->setPosition(gObj->getPosition());
			}

			//cout << glm::to_string(gsn->gameObjects[0].getPosition()) << endl;
			if (gObj->getId() == playerId) {
				this->camera->setPosition(gObj->getPosition());
			}

		}
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

	Draw::cleanUp();
}

Camera *Game::getCamera() const {
	return camera;
}

void Game::updateScreenDimensions(int width, int height) {
	screenWidth = width;
	screenHeight = height;
	textRenderer->updateScreenDimensions(width, height);
}

void Game::update(float dt) {
	float mouseMoveScale = mouseSensitivity * 0.001f;
	theta += (float)Input::getMouseDeltaX() * mouseMoveScale;
	phi += (float)Input::getMouseDeltaY() * mouseMoveScale;

	camera->setEyeAngles(vec3(-phi, theta, 0));

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

	// bytes of input bits to be sent to server
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
	if (glm::length(direction) != 0) {
		//direction = glm::normalize(direction) * dt * 5.0f;
		camera->setPosition(camera->getPosition() + direction);
	}

	if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
		shouldExit = true;
	}

	tuple<int, float, float> allInput(keyInputs, theta, phi);
	// Sending player input 
	NetBuffer buffer(NetMessage::PLAYER_INPUT);
	buffer.write< tuple<int,float,float> >(allInput);
	Network::send(buffer);

	// Arrow keys to move the ball.
	float ballDX = 0.0f;
	if (Input::isKeyDown(GLFW_KEY_LEFT)) {
		ballDX += dt * 5.0f;
	}
	if (Input::isKeyDown(GLFW_KEY_RIGHT)) {
		ballDX -= dt * 5.0f;
	}

	if (ballDX != 0.0f) {
		NetBuffer buffer(NetMessage::BALL_X);
		buffer.write<float>(ballDX);
		Network::send(buffer);
	}

	const auto curTime = (float)glfwGetTime();
	for (auto gameObject : gameObjects) {
		if (!gameObject) { continue; }
		gameObject->updateAnimation(curTime);
	}
}

void Game::drawScene(Shader &shader, DrawPass pass) const {
	for (auto gameObject : gameObjects) {
		if (!gameObject) { continue; }
		if (pass == DrawPass::LIGHTING) {
			testMat->bind(shader);
		}
		gameObject->draw(shader, camera);
	}
}

void Game::drawUI() const {
	textShader->use();
	textRenderer->renderText();
}

void Game::draw(float dt) const {
	// Shadow mapping render pass
	shadowMap->prePass();
	shadowMap->setupLight(shadowMap->getShader(), *sun);
	drawScene(shadowMap->getShader(), DrawPass::SHADOW);
	shadowMap->postPass();

	// Normal 3D render pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightShader->use();
	lightShader->setUniform("eyePos", camera->getPosition());
	lightShader->setUniform("directionalLightNum", 1);

	shadowMap->setupLight(*lightShader, *sun);
	shadowMap->bindTexture(*lightShader);

	sun->bind(*lightShader);
	drawScene(*lightShader, DrawPass::LIGHTING);
	skybox->draw();

	glDisable(GL_DEPTH_TEST);
	Draw::setupContext();
	drawUI();
	glEnable(GL_DEPTH_TEST);
}
