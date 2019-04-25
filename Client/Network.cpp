#include "Network.h"
#include <sstream>

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

void Network::send(const std::string &data) {
	if (socket) {
		std::stringstream ss(std::stringstream::binary);
		ss << data.size() << data;

		socket->send(boost::asio::buffer(ss.str()));
	}
}

void Network::cleanUp() {
	socket->close();
	delete socket;
	socket = nullptr;
}