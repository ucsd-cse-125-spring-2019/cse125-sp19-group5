#pragma once

#define BOOST_ALL_NO_LIB
#include <string>
#include <boost/asio.hpp>

using TcpSocket = boost::asio::ip::tcp::socket;

namespace Network {
	extern boost::asio::io_service ioService;
	extern TcpSocket *socket;

	void init(const std::string &address, int port);
	void send(const std::string &message);
	void poll();
	void cleanUp();
};

