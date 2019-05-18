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

/*
 * This is a function stub
 */
static MenuOptions menuPrompt(int playerId, MenuOptions currentMenuOptions) {
	int selection = 0;
	while (0 >= selection || selection >= 5)
	{
		cout << "-------------------------- " << endl;
		cout << "HERE ARE THE MENU OPTIONS: " << endl << endl;
		cout << "Team A (1):             " << (int)currentMenuOptions.team_A_1 << endl;
		cout << "Team A (2):             " << (int)currentMenuOptions.team_A_2 << endl << endl;
		cout << "Team B (1):             " << (int)currentMenuOptions.team_B_1 << endl;
		cout << "Team B (2):             " << (int)currentMenuOptions.team_B_2 << endl;
		cout << "-------------------------- " << endl;
		cout << "Type '1', '2', '3', or '4' to make a selection: ";
		cin >> selection;
		cout << endl;

		switch (selection) {
		case 1: currentMenuOptions.team_A_1 = playerId; break;
		case 2: currentMenuOptions.team_B_1 = playerId; break;
		case 3: currentMenuOptions.team_A_2 = playerId; break;
		case 4: currentMenuOptions.team_B_2 = playerId; break;
		}
	}
	return currentMenuOptions;
}

/*
 * This is a function stub
 */
static void clearMenuPrompt() {
	cout << "clearing the menu prompt!" << endl;
}

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

	int selectionComplete = 0;

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

	auto lastTime = (float)glfwGetTime();

	cout << "creating client" << endl;

	/*
	 * This is the callback function that is responsible for displaying all
	 * the menu options available before the game session has started.
	 * Currently there is only team selection support
	 */
	auto handleMenuInput = [&](Connection *server, NetBuffer &incomingMessage)
	{
		cout << "New Input Message...." << endl;
		//read in the current menu options from the server
		MenuOptions currentMenuOptions = incomingMessage.read<MenuOptions>();

		//else display these options and allow the client to make a selection
		game.setClientMenuOptions(menuPrompt(game.getPlayerId(), currentMenuOptions));
		NetBuffer menuInput(NetMessage::MENU_INPUT);
		menuInput.write<MenuOptions>(game.getClientMenuOptions());

		//send the client's selections to the server for confirmation
		server->send(menuInput);
	};

	/*
	 * This is the callback function that is responsible for handling confirmation
	 * from the server about the menu options selected by a client
	 */
	auto handleMenuConfirmed = [&](Connection *server, NetBuffer &serverMenuOptions)
	{
		cout << "server sent confirmation" << endl;
		MenuOptions currentMenuOptions = serverMenuOptions.read<MenuOptions>();


		game.setCurrentMenuOptions(currentMenuOptions);

		// Now we check if the Server has confirmed our selection or if it was 
		// confirming for another client instead
		if (!selectionComplete) {
			cout << "selectionComplete: " << selectionComplete << endl;
			if (game.serverConfirmed(currentMenuOptions))
			{
				// The game is ready to start, so clear the Menu prompt from the screen
				clearMenuPrompt();
				selectionComplete = 1;
			}
			else //The client selection window needs to update to reflect other player's selection
			{
				//display these options and allow the client to make a selection
				MenuOptions selectedOptions = menuPrompt(game.getPlayerId(), currentMenuOptions);

				////save these options
				game.setClientMenuOptions(selectedOptions);

				NetBuffer menuInput(NetMessage::MENU_INPUT);
				menuInput.write<MenuOptions>(game.getClientMenuOptions());
				//clearMenuPrompt();
				//send the client's selections to the server for confirmation
				server->send(menuInput);
			}
		}
	};

	// this handles the broadcast for the general menu option negotiations 
	// with the server (server will send a MENU_CONFRIM when accepted)
	//Network::onConnected([&]() {
	//	
	//	Network::on(NetMessage::MENU_INPUT, handleMenuInput);//callback for the client reply
	//	/*debug*/
	//	cout << "send menu options" << endl;
	//	/*end_debug*/

	//	// receive player keyboard and mouse(todo) input
	//	Network::onDisconnected([&]() {
	//		std::cout << "player has disconnected."
	//			<< std::endl;
	//	});
	//});

	// this is the server confirming a new selection to all connected clients 
	Network::on(NetMessage::MENU_OPTIONS, handleMenuInput);
	Network::on(NetMessage::MENU_CONFIRM, handleMenuConfirmed);

	while (1) {
		Network::poll();
	}
	// Main loop /* TODO: re-enable */
	while (1 == 2) {//!glfwWindowShouldClose(window)) {
		auto dt = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();

		Input::poll();
		Network::poll();

		if (selectionComplete) {
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
	}

	Network::cleanUp();
	glfwTerminate();
	return 0;
}