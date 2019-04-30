#include <iostream>
#include <Shared/Networking/Network.h>
#include "Server.h"

tcp::endpoint *Network::endpoint = nullptr;
tcp::acceptor *Network::acceptor = nullptr;
Connection* Network::connections[MAX_PLAYERS]; // Array of active connections.

tcp::socket *newSocket = nullptr; // Reserve a socket for the next new connection.

std::vector<Network::ConnectionHandler> connectionHandlers;

int Network::findFreeId() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!connections[i]) {
			return i;
		}
	}
	return -1;
}

void Network::handleClientDisconnect(Connection *connection) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (connections[i] == connection) {
			// TODO: change this so the connection is deleted in the next tick
			// since other handlers still need the connection to exist.
			//delete connections[i];
			connections[i] = nullptr;
			break;
		}
	}
}

void Network::handleIncomingConnection(const error_code &error) {
	if (error) {
		std::cerr << error.message() << std::endl;
		delete newSocket; newSocket = nullptr;
		acceptIncomingConnections();
		return;
	}

	auto id = findFreeId();

	// If the game is full, reject the connection.
	if (id < 0) {
		newSocket->close();
		delete newSocket; newSocket = nullptr;
		acceptIncomingConnections();
		return;
	}

	auto connection = new Connection(id, newSocket);
	connections[id] = connection;
	connection->onDisconnected(handleClientDisconnect);

	for (auto &handler : connectionHandlers) {
		handler(connection);
	}

	acceptIncomingConnections();
}

void Network::acceptIncomingConnections() {
	newSocket = new tcp::socket(ioService);
	acceptor->async_accept(*newSocket, handleIncomingConnection);
}

void Network::init(int port) {
	endpoint = new tcp::endpoint(tcp::v4(), port);
	acceptor = new tcp::acceptor(ioService, *endpoint);
	acceptIncomingConnections();
}

void Network::broadcast(NetBuffer &buffer) {
	for (auto &connection : connections) {
		if (connection) {
			connection->send(buffer);
		}
	}
}

void Network::broadcast(NetMessage message, const Serializable &obj) {
	for (auto &connection : connections) {
		if (connection) {
			connection->send(message, obj);
		}
	}
}

void Network::onClientConnected(ConnectionHandler handler) {
	connectionHandlers.push_back(handler);
}