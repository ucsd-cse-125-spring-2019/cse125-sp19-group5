#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Shared/ConfigSettings.h>
#include "Networking/Client.h"
#include "Game.h"
#include "Renderer/Camera.h"
#include "Renderer/Gui/Gui.h"
#include "Input.h"
#include "Renderer/Draw.h"
#include "Game/Gui/MenuPrompt.h"

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
static MenuOptions menuPrompt_old(int playerId, MenuOptions currentMenuOptions) {
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

// Update the view port when the window has been resized.
static void onResize(GLFWwindow *window, int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	SCREEN_RESHAPED = true;

	Draw::updateScreenDimensions(width, height);
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	bool selectionComplete = false;

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

	Draw::updateScreenDimensions(
		(float)SCREEN_WIDTH,
		(float)SCREEN_HEIGHT
	);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	auto glewInitRes = glewInit();
	glGetError();
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
	//Gui::setupInputListeners(window);
	//MenuPrompt menuPrompt;
	/*while (!menuPrompt.getConnected()) {
		menuPrompt.ipPrompt();
	}
	menuPrompt.settingsPrompt();*/

	Game game;
	MenuPrompt *menuPrompt = nullptr;

	game.getCamera()->setAspect((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	game.updateScreenDimensions(SCREEN_WIDTH, SCREEN_HEIGHT);

	auto lastTime = (float)glfwGetTime();

	cout << "creating client" << endl;

	/*
	 * This is the callback function that is responsible for displaying all
	 * the menu options available before the game session has started.
	 * Currently there is only team selection support
	 */
	auto handleMenuOptions = [&](Connection *server, NetBuffer &incomingMessage)
	{
		//read in the current menu options from the server 
		MenuOptions currentMenuOptions = incomingMessage.read<MenuOptions>();

		//update the client to match the server
		game.setCurrentMenuOptions(currentMenuOptions);

		menuPrompt = Gui::create<MenuPrompt>();

		menuPrompt->setGame(&game);

		//display these options and allow the client to make a selection
		//MenuOptions clientMenuOptions = menuPrompt(game.getPlayerId(), currentMenuOptions);

		//game.setClientMenuOptions(clientMenuOptions);
		//send the client's selections to the server for confirmation
		//NetBuffer menuInput(NetMessage::MENU_INPUT);
		//menuInput.write<MenuOptions>(game.getClientMenuOptions());
		//server->send(menuInput);
	};

	/*
	 * This is the callback function that is responsible for handling confirmation
	 * from the server about the menu options selected by a client
	 */
	/*
	auto handleMenuConfirmed = [&](Connection *server, NetBuffer &serverMessage)
	{
		cout << "server sent confirmation" << endl;
		int confirmedID = serverMessage.read<int>();
		MenuOptions currentMenuOptions = serverMessage.read<MenuOptions>();

		//update the client to match the server
		game.setCurrentMenuOptions(currentMenuOptions);
		

		if (confirmedID == game.getPlayerId())
		{
			//This client was confirmed
			clearMenuPrompt();
			
			//Start the game
			selectionComplete = true;
		}
		else //Some other client was confirmed
		{
			if (!selectionComplete) //this client has not yet been confirmed
			{

				//display new options and allow the client to make a selection
				//MenuOptions selectedOptions = 
				updateMenuPrompt(game.getPlayerId(), currentMenuOptions);

				//save these options
				//game.setClientMenuOptions(selectedOptions);

				//send the client's selections to the server for confirmation
				//NetBuffer menuInput(NetMessage::MENU_INPUT);
				//menuInput.write<MenuOptions>(game.getClientMenuOptions());

				//server->send(menuInput);
			}
		}


		// Now we check if the Server has confirmed our selection or if it was 
		// confirming for another client instead
		if (!selectionComplete) {
			cout << "selectionComplete: " << selectionComplete << endl;
			if (game.serverConfirmed(currentMenuOptions))
			{
				// The game is ready to start, so clear the Menu prompt from the screen
				clearMenuPrompt();

				//and start the game loop
				selectionComplete = 1;
			}
			else //The client selection window needs to update to reflect other player's selection
			{
			}
		}
	};*/

	// this is the server confirming a new selection to all connected clients 
	Network::on(NetMessage::MENU_OPTIONS,handleMenuOptions);
	//Network::on(NetMessage::MENU_CONFIRM, handleMenuConfirmed);


	// Main loop /* TODO: re-enable */
	while (1) {//!glfwWindowShouldClose(window)) {
		auto dt = (float)glfwGetTime() - lastTime;
		lastTime = (float)glfwGetTime();

		Input::poll();

		if (Network::connection) {
			Network::poll();
		}

		if (menuPrompt->getSelectionComplete()) {
			if (game.shouldExit) {
				glfwSetWindowShouldClose(window, true);
			}

			if (glfwWindowShouldClose(window)) {
				break;
			}

		game.update(dt);
		game.draw(dt);
		Gui::update(dt);

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