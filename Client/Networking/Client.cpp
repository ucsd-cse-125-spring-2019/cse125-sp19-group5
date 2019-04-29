#include "Client.h"
#include <iostream>

TcpSocket *Network::socket = nullptr;
Connection *Network::connection = nullptr;

std::vector<Network::ConnectedHandler> Network::connectedHandlers;
std::vector<Network::DisconnectedHandler> Network::disconnectedHandlers;
std::vector<std::pair<NetMessage, Connection::MessageHandler>> pendingHandlers;

bool Network::init(const std::string &address, int port) {
	try {
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(address),
			port
		);

		socket = new TcpSocket(ioService);
		socket->connect(endpoint);
		socket->non_blocking(true);

		connection = new Connection(-1, socket);

		for (auto &args : pendingHandlers) {
			on(args.first, args.second);
		}
		pendingHandlers.clear();

		return true;
	} catch (const boost::system::system_error& error) {
		std::cerr << "Failed to connect to " << address << ":" << port
			<< std::endl << error.what() << std::endl;
	}
	return false;
}

void Network::on(NetMessage message, Connection::MessageHandler handler) {
	if (connection) {
		connection->on(message, handler);
	} else {
		pendingHandlers.push_back(std::make_pair(message, handler));
	}
}

void Network::send(NetBuffer &buffer) {
	if (connection) {
		connection->send(buffer);
	}
}

void Network::send(NetMessage message, const Serializable &obj) {
	if (connection) {
		connection->send(message, obj);
	}
}

void Network::onConnected(ConnectedHandler handler) {
	connectedHandlers.push_back(handler);
}

void Network::onDisconnected(DisconnectedHandler handler) {
	disconnectedHandlers.push_back(handler);
}

void Network::cleanUp() {
	if (connection) {
		delete connection;
		connection = nullptr;
	}
}