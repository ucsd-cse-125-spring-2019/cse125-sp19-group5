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
std::vector<tcp::socket*> sockets;
tcp::socket *newSocket = nullptr;

constexpr auto TICKS_PER_SECOND = 60;

char messageData[128];

void receiveHandler(std::error_code ec, std::size_t bytes) {
	std::cout << ec.message() << std::endl;
	std::cout << "Received message of size " << bytes << std::endl;
		std::string blah;
}

void handleIncomingConnections();
void readMessageHeader(tcp::socket *socket);

void onReceiveMessageHeader(
	tcp::socket *socket,
	const boost::system::error_code& error,
	std::size_t bytes
) {
	if (error) {
		std::cerr << error.message() << std::endl;
		return;
	}

	std::cout << "Received " << bytes << " bytes long message" << std::endl;
	readMessageHeader(socket);
}

void readMessageHeader(tcp::socket *socket) {
	boost::asio::streambuf buffer;
	/*
	// TODO (bhang): change this to support a header size
	socket->async_read_some(
		boost::asio::buffer(messageData, 8),
		[socket](const boost::system::error_code &error, std::size_t bytes) {
			onReceiveMessageHeader(socket, error, bytes);
		}
	);
	*/
	boost::asio::async_read_until(
		*socket,
		buffer,
		"\n",
		[socket](const boost::system::error_code &error, std::size_t bytes) {
			onReceiveMessageHeader(socket, error, bytes);
		}
	);
}

void onAccept(const std::error_code& error) {
	if (!error) {
		std::cout << "A client has connected!" << std::endl;
	} else {
		std::cout << error.message() << std::endl;
	}

	readMessageHeader(newSocket);

	sockets.push_back(newSocket);
	handleIncomingConnections();
}

void handleIncomingConnections() {
	newSocket = new tcp::socket(ioService);
	acceptor.async_accept(
		*newSocket,
		onAccept
	);
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