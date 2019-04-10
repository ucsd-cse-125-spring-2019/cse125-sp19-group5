#include "Game.h"
#include "Renderer/Common.h"
#include <glm/gtx/transform.hpp>
#include "Input.h"

Game::Game() {
	lightShader = new Shader("Shaders/light");
	bear = new Model("Models/ucsd-bear-sp10.obj");
	sphere = new Model("Models/sphere.obj");
	camera = new Camera(vec3(-7.5f, 2.5f, 0.0f), vec3(0.0f), 70, 1.0f);
	sun = new DirectionalLight(0);
	sun->setAmbient(vec3(0.04f, 0.05f, 0.13f));
	sun->setColor(vec3(0.8f, 0.7f, 0.55f));

	white = new Texture2d("Textures/white.png");
	grass = new Texture2d("Textures/grass.png");

	Input::setMouseVisible(false);
}

Game::~Game() {
	delete lightShader;
	delete bear;
	delete camera;
	delete sun;
}

Camera *Game::getCamera() const {
	return camera;
}

void Game::update(float dt) {
	theta += dt * (float)Input::getMouseDeltaX() * 0.1f;
	phi += dt * (float)Input::getMouseDeltaY() * 0.1f;

	camera->setEyeAngles(vec3(-phi, theta, 0));

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
}

void Game::draw(float dt) const {
	auto model = mat4(1.0f);
	model = glm::scale(model, vec3(0.3f));
	// model = glm::rotate(model, theta, vec3(0.0f, 1.0f, 0.0f));
	// model = glm::rotate(model, phi, vec3(1.0f, 0.0f, 0.0f));

	auto modelInvT = glm::transpose(glm::inverse(mat3(model)));

	// TODO (bhang): refactor this to some sort of renderer class so the game
	// doesn't have to deal with all of this transformation + shader stuff?
	white->bind(0);
	lightShader->use();
	lightShader->setUniform("eyePos", camera->getPosition());
	lightShader->setUniform("modelInvT", modelInvT);
	lightShader->setUniform("mvp", camera->getMatrix() * model);
	lightShader->setUniform("model", model);
	lightShader->setUniform("directionalLightNum", 1);


	sun->bind(*lightShader);
	bear->draw(*lightShader);
	grass->bind(0);

	model = mat4(1.0f);
	model = glm::scale(model, vec3(0.1f));
	model = glm::translate(model, vec3(50.0f, 25.0f, 0.0f));
	modelInvT = glm::transpose(glm::inverse(mat3(model)));
	lightShader->setUniform("modelInvT", modelInvT);
	lightShader->setUniform("mvp", camera->getMatrix() * model);
	lightShader->setUniform("model", model);
	sphere->draw(*lightShader);
}
