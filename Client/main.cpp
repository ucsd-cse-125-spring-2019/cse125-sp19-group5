#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Shared/ConfigSettings.h>
#include "Networking/Client.h"
#include "Game.h"
#include "Renderer/Camera.h"
#include "Input.h"

using std::string;
using std::cout;
using std::endl;

constexpr auto TITLE = "Insert Title Here";

auto SCREEN_WIDTH = 800;
auto SCREEN_HEIGHT = 600;
auto SCREEN_RESHAPED = false;


// Update the view port when the window has been resized.
static void onResize(GLFWwindow *window, int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	SCREEN_RESHAPED = true;

	glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	ConfigSettings::get().getValue("ScreenWidth", SCREEN_WIDTH);
	ConfigSettings::get().getValue("ScreenHeight", SCREEN_HEIGHT);

	// Create a window with OpenGL 3.3 core.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	auto window = glfwCreateWindow(
		SCREEN_WIDTH, SCREEN_HEIGHT,
		TITLE,
		nullptr, nullptr
	);
	if (!window) {
		std::cerr << "Failed to create window!" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	auto glewInitRes = glewInit();
	if (glewInitRes != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl
			<< glewGetErrorString(glewInitRes) << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onResize); // Resize callback
	glfwSwapInterval(1); // Vertical sync

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE); 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Center the window
	auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	auto middleX = (videoMode->width - SCREEN_WIDTH) / 2;
	auto middleY = (videoMode->height - SCREEN_HEIGHT) / 2;
	glfwSetCursorPos(window, middleX, middleY);
	glfwSetWindowPos(window, middleX, middleY);

	Input::init(window);
	Network::init("127.0.0.1", 1234);

	Game game;


	game.getCamera()->setAspect((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	game.updateScreenDimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << endl;
    }

	auto lastTime = (float)glfwGetTime();

	cout << "creating client" << endl;

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		auto dt = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();

		Input::poll();
		Network::poll();

		if (game.shouldExit) {
			glfwSetWindowShouldClose(window, true);
		}

		if (glfwWindowShouldClose(window)) {
			break;
		}

		game.update(dt);
		game.draw(dt);

		glfwSwapBuffers(window);

		if (SCREEN_RESHAPED) {
			game.getCamera()->setAspect(
				(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
			);
			game.updateScreenDimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
			SCREEN_RESHAPED = false;
		}
	}

	Network::cleanUp();
	glfwTerminate();
	return 0;
}