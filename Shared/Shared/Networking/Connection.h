#pragma once

#include <boost/bind.hpp>
#include <unordered_map>
#include <vector>
#include "Network.h"
#include "NetMessage.h"
#include "NetBuffer.h"
#include "Serializable.h"

using std::unordered_map;
using std::vector;
using ErrorCode = boost::system::error_code;

constexpr auto MAX_MESSAGE_LENGTH = 32768; // bytes (32KB)

class Connection {
	public:
	using MessageHandler = std::function<
		void(Connection*, NetBuffer &buffer)
	>;
	using DisconnectHandler = std::function<void(Connection*)>;

	private:
	char messageData[MAX_MESSAGE_LENGTH];
	TcpSocket *socket = nullptr;
	unordered_map<NetMessage, vector<MessageHandler>> handlers;
	std::vector<DisconnectHandler> disconnectHandlers;
	NetBufferHeader curHeader;
	int id = 0;

	void readHeader();
	void readBuffer(NetBufferHeader header);
	void handleHeader(const ErrorCode &error, size_t bytes);
	void handleBuffer(
		NetBufferHeader header,
		const ErrorCode &error,
		size_t bytes
	);
	bool handleDisconnect(const ErrorCode &error);

	public:
	Connection(int id, TcpSocket *socket);
	void on(NetMessage message, MessageHandler handler);
	void onDisconnected(DisconnectHandler handler);
	void send(NetBuffer &buffer);
	void send(NetMessage message, const Serializable &obj);
	void close();
	int getId() const;
};