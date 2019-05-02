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

Game::Game() {
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

	ClientGameObject *ball = new ClientGameObject("ball");
	ball->setModel("Models/sphere.obj");
	ball->setScale(vec3(0.2f));
	gameObjects.push_back(ball);

	Network::on(NetMessage::GAME_STATE_UPDATE, [](Connection *c, NetBuffer &buffer) {
		GameStateNet currentGameState = buffer.read<GameStateNet>();
		/*TODO: graphics update based on the game state*/
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
	int inputs = 0;
	vec3 direction(0.0f);
	if (Input::isKeyDown(GLFW_KEY_W)) {
		direction += camera->getForward();
		inputs += FORWARD;
	}
	if (Input::isKeyDown(GLFW_KEY_S)) {
		direction -= camera->getForward();
		inputs += BACKWARD;
	}
	if (Input::isKeyDown(GLFW_KEY_A)) {
		direction -= camera->getRight();
		inputs += LEFT;
	}
	if (Input::isKeyDown(GLFW_KEY_D)) {
		direction += camera->getRight();
		inputs += RIGHT;
	}
	if (glm::length(direction) != 0) {
		direction = glm::normalize(direction) * dt * 5.0f;
		camera->setPosition(camera->getPosition() + direction);
	}

	if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
		shouldExit = true;
	}

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
		gameObject->updateAnimation(curTime);
	}
}

void Game::drawScene(Shader &shader) const {
	white->bind(0);

	for (auto gameObject : gameObjects) {
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
	drawScene(shadowMap->getShader());
	shadowMap->postPass();

	// Normal 3D render pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightShader->use();
	lightShader->setUniform("eyePos", camera->getPosition());
	lightShader->setUniform("directionalLightNum", 1);

	shadowMap->setupLight(*lightShader, *sun);
	shadowMap->bindTexture(*lightShader);

	sun->bind(*lightShader);
	drawScene(*lightShader);
	skybox->draw();

	glDisable(GL_DEPTH_TEST);
	Draw::setupContext();
	drawUI();
	glEnable(GL_DEPTH_TEST);
}
