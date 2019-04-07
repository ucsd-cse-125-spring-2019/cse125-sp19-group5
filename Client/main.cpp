#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	// Create a window with OpenGL 3.3 core.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);                

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
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Center the window
	auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(
		window,
		(videoMode->width - SCREEN_WIDTH) / 2,
		(videoMode->height - SCREEN_HEIGHT) / 2
	);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (glfwWindowShouldClose(window)) {
			break;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);

		if (SCREEN_RESHAPED) {
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SCREEN_RESHAPED = false;
		}
	}

	glfwTerminate();
	return 0;
}