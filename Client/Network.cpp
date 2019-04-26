#include "Network.h"
#include <sstream>

boost::asio::io_service Network::ioService;
TcpSocket *Network::socket = nullptr;

void Network::init(const std::string &address, int port) {
	try {
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(address),
			port
		);

		socket = new TcpSocket(ioService);
		socket->connect(endpoint);
		socket->non_blocking(true);
	} catch (std::runtime_error e) {
		std::cerr << "Failed to connect to server!" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void Network::poll() {
	ioService.poll();
}

void Network::send(const std::string &data) {
	if (socket) {
		socket->send(
			boost::asio::buffer(data + "\n")
		);
	}
}

void Network::cleanUp() {
	if (socket) {
		socket->close();
		delete socket;
		socket = nullptr;
	}
}