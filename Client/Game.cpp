#include "Game.h"
#include <Shared/Common.h>
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
	bear = new Model("Models/ground.obj");
	sphere = new Model("Models/sphere.obj");
	sphere->setAnimation(0);
	camera = new Camera(vec3(-7.5f, 2.5f, 0.0f), vec3(0.0f), 70, 1.0f);
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

	audioPlayer = new AudioPlayer();
	audioPlayer->playLoop("Sounds/minecraft_wet_hands.wav");

	Network::on(NetMessage::TEST, [](Connection *c, NetBuffer &buffer) {
		GameStateNet gs;
		gs.deserialize(buffer);
		std::cout << gs.timeLeft << std::endl;
	});
}

Game::~Game() {
	delete lightShader;
	delete textShader;
	delete textRenderer;
	delete bear;
	delete camera;
	delete sun;
	delete shadowMap;

	Draw::cleanUp();
}

Camera *Game::getCamera() const {
	return camera;
}


int num = 0;

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

	fpsTextTimer += dt;
	int fps = (int) (1.0f / dt);
	if (fpsTextTimer > 0.3f)
	{
		fpsTextTimer = 0.0f;
		fpsText->text = "fps: " + std::to_string(fps);
	}

	vec3 direction(0.0f);
	if (Input::isKeyDown(GLFW_KEY_W)) {
		direction += camera->getForward();
	}
	if (Input::isKeyDown(GLFW_KEY_S)) {
		direction -= camera->getForward();
	}
	if (Input::isKeyDown(GLFW_KEY_A)) {
		direction -= camera->getRight();
	}
	if (Input::isKeyDown(GLFW_KEY_D)) {
		direction += camera->getRight();
	}
	if (glm::length(direction) != 0) {
		direction = glm::normalize(direction) * dt * 5.0f;
		camera->setPosition(camera->getPosition() + direction);
	}

	if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
		shouldExit = true;
	}

	if (Input::wasKeyPressed(GLFW_KEY_P)) {
		std::cout << num << std::endl;
		num++;
	}

	if (Input::isKeyDown(GLFW_KEY_LEFT)) {
		ballX += dt * 5.0f;
	}
	if (Input::isKeyDown(GLFW_KEY_RIGHT)) {
		ballX -= dt * 5.0f;
	}

	sphere->updateAnimation((float)glfwGetTime());
}

void Game::drawScene(Shader &shader) const {
	auto model = mat4(1.0f);
	model = glm::translate(model, vec3(ballX, 0.5f, 0.0f));
	model = glm::scale(model, vec3(0.2f));
	auto modelInvT = glm::transpose(glm::inverse(mat3(model)));

	shader.setUniform("model", model);
	shader.setUniform("modelInvT", modelInvT);
	shader.setUniform("mvp", camera->getMatrix() * model);

	white->bind(0);
	sphere->draw(shader);

	model = mat4(1.0f);
	modelInvT = glm::transpose(glm::inverse(model));
	shader.setUniform("model", model);
	shader.setUniform("modelInvT", modelInvT);
	shader.setUniform("mvp", camera->getMatrix() * model);

	bear->draw(shader);
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
