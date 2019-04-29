#pragma once

#include <Shared/Networking/Network.h>
#include <Shared/Networking/NetBuffer.h>
#include <Shared/Networking/Serializable.h>
#include <Shared/Networking/Connection.h>

namespace Network {
	using ConnectedHandler = std::function<void()>;
	using DisconnectedHandler = std::function<void()>;

	extern Connection *connection;
	extern TcpSocket *socket;
	extern std::vector<ConnectedHandler> connectedHandlers;
	extern std::vector<DisconnectedHandler> disconnectedHandlers;

	bool init(const std::string &address, int port);
	void send(const NetBuffer &buffer);
	void send(NetMessage message, const Serializable &obj);
	void on(NetMessage message, Connection::MessageHandler handler);
	void onConnected(ConnectedHandler handler);
	void onDisconnected(DisconnectedHandler handler);
	void cleanUp();
}