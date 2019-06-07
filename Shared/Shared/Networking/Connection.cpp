#include "Connection.h"
#include <iostream>

static NetMessage lastGoodHeader = NetMessage::TEST;

Connection::Connection(int id, TcpSocket *socket) : id(id), socket(socket) {
	readHeader();
	socket->set_option(boost::asio::socket_base::keep_alive(true));
}

bool Connection::handleDisconnect(const ErrorCode &error) {
	if (
		error == boost::asio::error::eof ||
		error == boost::asio::error::connection_refused ||
		error == boost::asio::error::connection_reset
	) {
		for (auto &handler : disconnectHandlers) {
			handler(this);
		}
		disconnectHandlers.clear();
		return true;
	}
	return false;
}

void Connection::readHeader() {
	socket->async_read_some(
		boost::asio::buffer(messageData, sizeof(NetBufferHeader)),
		boost::bind(&Connection::handleHeader, this, _1, _2)
	);
}

void Connection::readBuffer(NetBufferHeader header) {
	socket->async_read_some(
		boost::asio::buffer(messageData, header.size - sizeof(NetBufferHeader)),
		boost::bind(&Connection::handleBuffer, this, header, _1, _2)
	);
}

void Connection::handleHeader(const ErrorCode &error, size_t bytes) {
	if (handleDisconnect(error)) { return; }
	if (bytes != sizeof(NetBufferHeader)) {
		std::cerr << "BAD HEADER READ!" << std::endl;
		std::cerr << "LAST GOOD HEADER: " << lastGoodHeader << std::endl;
		throw std::exception("Bad header read");
	}
	NetBufferHeader header;
	std::memcpy(&header, messageData, sizeof(NetBufferHeader));
	std::cerr << header.message << std::endl;
	if (header.message <= 0 || header.message >= NetMessage::numValues) {
		std::cerr << "BAD MESSAGE" << std::endl;
		std::cerr << "LAST GOOD HEADER: " << lastGoodHeader << std::endl;
		throw std::exception("Bad message");
	}
	readBuffer(header);
}


void Connection::handleBuffer(
	NetBufferHeader header,
	const ErrorCode &error,
	size_t bytes
) {
	if (handleDisconnect(error)) { return; }
	lastGoodHeader = (NetMessage)header.message;
	NetMessage message = (NetMessage)header.message;
	NetBuffer buffer(message, NetBufferType::READ);
	buffer.setData(messageData, header.size);

	if (handlers.find(message) != handlers.end()) {
		for (auto &handler : handlers[message]) {
			handler(this, buffer);
		}
	}

	readHeader();
}

void Connection::on(NetMessage message, MessageHandler handler) {
	if (handlers.find(message) == handlers.end()) {
		handlers[message] = vector<MessageHandler>();
	}
	handlers[message].push_back(handler);
}

void Connection::onDisconnected(DisconnectHandler handler) {
	disconnectHandlers.push_back(handler);
}

void Connection::send(NetBuffer &buffer) {
	if (!socket || !socket->is_open()) { return; }
	if (!buffer.isFinished()) {
		buffer.finish();
	}

	std::shared_ptr<std::string> data = std::make_shared<std::string>(
		buffer.getData()
	);

	const auto message = buffer.getMessageType();
	socket->async_send(
		boost::asio::buffer(*data, buffer.getSize()),
		[this, message, data](auto error, auto bytes) {
		if (handleDisconnect(error)) { return; }
		if (error) {
			std::cerr << "Failed to send message " << message << std::endl;
			std::cerr << error.message() << std::endl;
		}
		}
	);
}

void Connection::send(NetMessage message, const Serializable &obj) {
	NetBuffer buffer(message);
	obj.serialize(buffer);
	buffer.finish();
	send(buffer);
}

void Connection::close() {
	if (socket) {
		socket->close();
	}
}

int Connection::getId() const {
	return id;
}
