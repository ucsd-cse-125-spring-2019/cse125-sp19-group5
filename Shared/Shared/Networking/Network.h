#pragma once
#define BOOST_ALL_NO_LIB
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include "../CommonStructs.h"

using TcpSocket = boost::asio::ip::tcp::socket;

namespace Network {
	extern boost::asio::io_service ioService;
	extern TcpSocket *socket;

	void init(const std::string &address, int port);

	void read(int length = 4);//addded ability to read Server Updates

	void send(const std::string &data);
	void poll();
	void cleanUp();
};

