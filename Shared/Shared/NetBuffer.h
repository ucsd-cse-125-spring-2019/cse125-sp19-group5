#pragma once

#include <sstream>

enum NetBufferType {
	READ,
	WRITE
};

class NetBuffer {
	private:
	std::stringbuf buf;
	int32_t size;
	public:
	NetBuffer(NetBufferType type = NetBufferType::WRITE): size(0) {
		// Allocate size header if we will send this buffer (i.e. type = write).
		if (type != NetBufferType::READ) {
			write<int32_t>(0);
		}
	}

	void setData(char *data, int size) {
		this->size = size;
		buf.sputn(data, size);
	}

	template<typename T>
	inline void write(const T &value) {
		buf.sputn(reinterpret_cast<const char*>(&value), sizeof(T));
		size += sizeof(T);
	}

	template<typename T>
	inline T read() {
		char data[sizeof(T)] = { 0 };
		buf.sgetn(data, sizeof(T));
		return *reinterpret_cast<T*>(data);
	}

	void finish() {
		buf.pubseekpos(0);
		write<int32_t>(size);
	}

	std::string getData() const {
		return buf.str();
	}

	int32_t getSize() {
		return size;
	}
};

