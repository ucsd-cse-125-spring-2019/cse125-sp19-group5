#include "Game.h"
#include <Shared/Common.h>
#include <Shared/ConfigSettings.h>
#include <glm/gtx/transform.hpp>
#include "Input.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Game::Game() {
	lightShader = new Shader("Shaders/light");
	bear = new Model("Models/boblampclean.md5mesh");
	sphere = new Model("Models/sphere.obj");
	camera = new Camera(vec3(-7.5f, 2.5f, 0.0f), vec3(0.0f), 70, 1.0f);
	sun = new DirectionalLight(0);
	sun->setDirection(vec3(0.009395, -0.700647, -0.713446));
	sun->setAmbient(vec3(0.04f, 0.05f, 0.13f));
	sun->setColor(vec3(0.8f, 0.7f, 0.55f));

	skybox = new Skybox("Textures/Skybox/cloudtop", *camera);

	white = new Texture2d("Textures/white.png");
	grass = new Texture2d("Textures/grass.png");

	Input::setMouseVisible(false);

	ConfigSettings::get().getValue("MouseSensitivity", mouseSensitivity);
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
	float mouseMoveScale = mouseSensitivity * 0.001f;
	theta += (float)Input::getMouseDeltaX() * mouseMoveScale;
	phi += (float)Input::getMouseDeltaY() * mouseMoveScale;

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

	if (Input::wasKeyPressed(GLFW_KEY_P)) {
		std::cout << glm::to_string(camera->getForward()) << std::endl;
	}

	bear->updateAnimation((float)glfwGetTime());
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

	model = glm::translate(vec3(5.0f, 0.0f, 0.0f))
		* glm::scale(model, vec3(0.5f));
	modelInvT = glm::transpose(glm::inverse(mat3(model)));
	lightShader->setUniform("modelInvT", modelInvT);
	lightShader->setUniform("mvp", camera->getMatrix() * model);
	lightShader->setUniform("model", model);
	sphere->draw(*lightShader);

	skybox->draw();
}
