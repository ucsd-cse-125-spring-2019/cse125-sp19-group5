#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <Shared/Networking/Connection.h>

constexpr auto MAX_CONNECTIONS = 256;

using boost::asio::ip::tcp;
using boost::system::error_code;

namespace Network {
	using ConnectionHandler = std::function<void(Connection*)>;

	extern tcp::endpoint *endpoint;
	extern tcp::acceptor *acceptor;
	extern Connection *connections[MAX_CONNECTIONS];

	void init(int port);
	void handleIncomingConnection(const error_code &error);
	void handleClientDisconnect(Connection *connection);
	void acceptIncomingConnections();
	void broadcast(NetBuffer &buffer);
	void broadcast(NetMessage message, const Serializable &obj);
	void onClientConnected(ConnectionHandler handler);
	int findFreeId();
}
