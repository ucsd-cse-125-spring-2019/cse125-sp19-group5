#pragma once

#include <sstream>
#include "NetMessage.h"

// The purpose of the NetBuffer.
enum NetBufferType {
	READ, // The buffer is being used to read data from somewhere.
	WRITE // The buffer is going to have data written to it.
};

// Information used to read a buffer when received from the network.
struct NetBufferHeader {
	int32_t size = 0;
	int16_t message = 0;
};

// A NetBuffer provides a way to read/write various data types into a buffer
// which can be sent over the network.
class NetBuffer {
	private:
	std::stringbuf buf; // Container for the actual data.
	NetBufferHeader header; // The header used to store metadata about the buffer.
	bool finished = false; // Whether or not finish() was called.

	public:
	NetBuffer(
		NetMessage message,
		NetBufferType type = NetBufferType::WRITE
	) {
		header.message = (int16_t)message;
		// Allocate size header if we will send this buffer (i.e. type = write).
		if (type != NetBufferType::READ) {
			char blank[sizeof(NetBufferHeader)];
			write<char*>(blank);
		}
	}

	// This sets the actual data stored in the buffer to the given data.
	// This is used when reading data from a socket into a NetBuffer.
	void setData(char *data, int32_t size) {
		header.size = size;
		buf.sputn(data, size);
	}
	
	// Writes data from the given value into the buffer.
	template<typename T>
	inline void write(const T &value) {
		if (finished) {
			throw std::exception("Cannot write to finished buffer");
		}
		buf.sputn(reinterpret_cast<const char*>(&value), sizeof(T));
		header.size += (int32_t)sizeof(T);
	}

	template<>
	inline void write(const std::string &value) {
		if (finished) {
			throw std::exception("Cannot write to finished buffer");
		}
		auto size = value.size();
		write(size);
		buf.sputn(value.c_str(), size);
		header.size += (int32_t)size;
	}

	// Reads the given data type from the beginning of the buffer, then moves
	// the beginning of the buffer to after the read data.
	template<typename T>
	inline T read() {
		char data[sizeof(T)] = { 0 };
		buf.sgetn(data, sizeof(T));
		return *reinterpret_cast<T*>(data);
	}

	template<>
	inline std::string read() {
		auto size = read<size_t>();
		auto data = new char[size];
		buf.sgetn(data, size);
		std::string res(data, size);
		delete data;
		return res;
	}

	// Writes the header to the beginning of the buffer, so the data can be read
	// from a socket later, and makes the buffer read only.
	void finish() {
		buf.pubseekpos(0);
		write<NetBufferHeader>(header);
		finished = true;
	}

	// Returns the data contained in this buffer as a string.
	std::string getData() const {
		return buf.str();
	}

	// Returns the amount of bytes written to the buffer.
	int32_t getSize() const {
		return header.size;
	}

	// Returns which message this buffer is suppose to be used for.
	NetMessage getMessageType() const {
		return (NetMessage)header.message;
	}

	// Returns true iff the finish() was called.
	bool isFinished() const {
		return finished;
	}
};

