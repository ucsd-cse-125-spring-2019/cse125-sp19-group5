#include "Network.h"

boost::asio::io_service Network::ioService;
TcpSocket *Network::socket = nullptr;

void Network::init(const std::string &address, int port) {
	boost::asio::ip::tcp::endpoint endpoint(
		boost::asio::ip::address::from_string(address),
		port
	);

	socket = new TcpSocket(ioService);
	socket->connect(endpoint);
	socket->non_blocking(true);
}

void Network::poll() {
	ioService.poll();
}

void Network::send(const std::string &message) {
	if (socket) {
		socket->send(boost::asio::buffer(message));
	}
}

void Network::cleanUp() {
	socket->close();
	delete socket;
	socket = nullptr;
}