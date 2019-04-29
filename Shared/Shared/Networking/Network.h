#pragma once
#define BOOST_ALL_NO_LIB
#include <string>
#include <sstream>
#include <boost/asio.hpp>

using TcpSocket = boost::asio::ip::tcp::socket;

namespace Network {
	extern boost::asio::io_service ioService;

	void poll();
};

