#define BOOST_ALL_NO_LIB
#include <boost/asio.hpp>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Shared/ConfigSettings.h>
#include "Game.h"
#include "Renderer/Camera.h"
#include "Input.h"

using namespace boost::asio;
using ip::tcp;
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

int client() {
	boost::asio::io_service io_service;
	//socket creation
	tcp::socket socket(io_service);
	//connection
	socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
	// request/message from client
	const string msg = "Hello from Client!\n";
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(msg), error);
	if (!error) {
		cout << "Client sent hello message!" << endl;
	}
	else {
		cout << "send failed: " << error.message() << endl;
	}
	// getting response from server
	boost::asio::streambuf receive_buffer;
	boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
	if (error && error != boost::asio::error::eof) {
		cout << "receive failed: " << error.message() << endl;
	}
	else {
		const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
		cout << data << endl;
	}
	return 0;
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
	glfwSetWindowPos(
		window,
		(videoMode->width - SCREEN_WIDTH) / 2,
		(videoMode->height - SCREEN_HEIGHT) / 2
	);

	Input::init(window);

	Game game;
	game.getCamera()->setAspect((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	auto lastTime = (float)glfwGetTime();

	client();

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		auto dt = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();

		Input::poll();

		if (game.shouldExit) {
			glfwSetWindowShouldClose(window, true);
		}

		if (glfwWindowShouldClose(window)) {
			break;
		}

		game.update(dt);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		game.draw(dt);

		glfwSwapBuffers(window);

		if (SCREEN_RESHAPED) {
			game.getCamera()->setAspect(
				(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
			);
			SCREEN_RESHAPED = false;
		}
	}

	glfwTerminate();
	return 0;
}