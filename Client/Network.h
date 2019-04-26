#pragma once
#define BOOST_ALL_NO_LIB
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <Shared\CommonStructs.h>

using TcpSocket = boost::asio::ip::tcp::socket;
using ByteVector = std::vector<unsigned char>;

class NetworkMsg {
	int id;
	std::stringstream buffer;

	public:
	NetworkMsg(int id) : id(id), buffer(std::stringstream::binary) { }

	void write(int v) {
		buffer << v;
	}
};

namespace Network {
	extern boost::asio::io_service ioService;
	extern TcpSocket *socket;

	void init(const std::string &address, int port);
	void send(const std::string &data);
	void poll();
	void cleanUp();
};

