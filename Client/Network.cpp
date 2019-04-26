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

		Network::read();
	} catch (std::runtime_error e) {
		std::cerr << "Failed to connect to server!" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

//the eventual place where data will go
constexpr size_t messageHeaderLength = sizeof(int32_t);
char theMessage[messageHeaderLength];

/*
 * This function will read in the server's response in a two-phase process.
 * First, socket->receive will put the incoming data onto a Boost::asio buffer, 
 * then from that buffer, we will transfer the data onto a "normal" buffer where
 * finally we write it into a normal looking string that we can print out.
 */
void Network::read() {
	if (!socket) { return; }
	socket->async_read_some(
		boost::asio::buffer(theMessage, messageHeaderLength),
		[](auto error, auto bytes) {
			int32_t *headerSize = reinterpret_cast<int32_t*>(theMessage);
			std::cout << *headerSize << std::endl;
			Network::read();
		}
	);
}

void Network::poll() {
	ioService.poll();
}

void Network::send(const std::string &data) {
	if (!socket) { return; }
	socket->send(boost::asio::buffer(data + "\n"), 0);
}

void Network::cleanUp() {
	if (!socket) { return; }
	socket->close();
	delete socket;
	socket = nullptr;
}