#include "Game.h"
#include "Renderer/Common.h"
#include <glm/gtx/transform.hpp>

Game::Game() {
	lightShader = new Shader("Shaders/light");
	bear = new Model("Models/ucsd-bear-sp10.obj");
	camera = new Camera(vec3(-7.5f, 2.5f, 0.0f), vec3(0.0f), 70, 1.0f);
	sun = new DirectionalLight(0);
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
	theta += dt * 1.5f;
}

void Game::draw(float dt) const {
	auto model = mat4(1.0f);
	model = glm::scale(model, vec3(0.3f));
	model = glm::rotate(model, theta, vec3(0.0f, 1.0f, 0.0f));

	auto modelInvT = glm::transpose(glm::inverse(model));

	// TODO (bhang): refactor this to some sort of renderer class so the game
	// doesn't have to deal with all of this transformation + shader stuff?
	lightShader->use();
	lightShader->setUniform("eyePos", camera->getPosition());
	lightShader->setUniform("modelInvT", modelInvT);
	lightShader->setUniform("mvp", camera->getMatrix() * model);
	lightShader->setUniform("directionalLightNum", 1);

	sun->bind(*lightShader);

	bear->draw(*lightShader);
}
