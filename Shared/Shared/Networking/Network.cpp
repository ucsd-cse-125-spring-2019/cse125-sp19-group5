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
constexpr size_t messageHeaderLength = 1024;
char theMessage[messageHeaderLength];

/*
 * This function will read in the server's response in a two-phase process.
 * First, socket->receive will put the incoming data onto a Boost::asio buffer, 
 * then from that buffer, we will transfer the data onto a "normal" buffer where
 * finally we write it into a normal looking string that we can print out.
 */
void Network::read(int length) {
	if (!socket) { return; }
	socket->async_read_some(
		boost::asio::buffer(theMessage, length),
		[length](auto error, auto bytes) {
			// TODO (bhang): change this if condition.
			// Reading messages from the server is done in 2 steps:
			//   1. Read a 4 byte (int32) length of the incoming message
			//   2. Then, read the next `length` bytes
			// So right now, when testing the header length is 4 bytes and
			// the server will send a message of length > 4. That's how we know
			// for now if we are going to read the header or not.


			// If length != 4 -> we are receiving a game state update.
			if (length != 4) {
				auto buf = NetBuffer(NetBufferType::READ);
				buf.setData(theMessage, (int32_t)bytes);
				GameStateNet gs;
				gs.deserialize(buf);
				std::cout << gs.timeLeft << std::endl;

				// After reading a game state update, expect a header (4 bytes).
				Network::read(4);
				return;
			}

			// If we didn't receive a game state update, then the message
			// we received must be a header (4 bytes).
			int32_t *headerSize = reinterpret_cast<int32_t*>(theMessage);
			std::cout << *headerSize << std::endl;

			// So, we get the amount of bytes we should expect next from the header.
			Network::read(*headerSize);
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