#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include "GameEngine.h"
#include <chrono>

using boost::asio::ip::tcp;

boost::asio::io_service ioService;
tcp::endpoint endpoint{ tcp::v4(), 1234 };
tcp::acceptor acceptor{ ioService, endpoint };
std::vector<tcp::socket> sockets;

constexpr auto TICKS_PER_SECOND = 60;

void receiveHandler(std::error_code ec, std::size_t bytes) {
	std::cout << ec.message() << std::endl;
	std::cout << "Received message of size " << bytes << std::endl;
		std::string blah;

		boost::asio::async_read(
			newSocket,
			boost::asio::buffer(blah, 4),
			receiveHandler
		);
}

void handleIncomingConnections() {
	acceptor.async_accept([](std::error_code ec, tcp::socket &&newSocket) {
		if (ec) {
			std::cerr << ec.message() << std::endl;
			handleIncomingConnections();
			return;
		}
		newSocket.non_blocking(true);
		sockets.push_back(std::move(newSocket));
		std::cout << "A client has connected!" << std::endl;
		handleIncomingConnections();

		std::string blah;

		boost::asio::async_read(
			newSocket,
			boost::asio::buffer(blah, 4),
			receiveHandler
		);
	});
}

int main(int argc, char **argv) {
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), "player_1", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_1", 1));

	handleIncomingConnections();

	while (true) {
		ioService.poll();

		vector<PlayerInputs> playerInputs;
		for (int i = 0; i < 4; i++) {
			gameEngine.updateGameState(playerInputs);
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000 / TICKS_PER_SECOND)
		);
	}

	return 0;
}