#pragma once

#include <sstream>

class NetBuffer {
	private:
	std::stringbuf buf;
	int32_t size;
	public:
	NetBuffer(): size(0) {
		write<int32_t>(0);
	}

	NetBuffer(const char *data): size(0) {
		buf = std::stringbuf(data);
	}

	template<typename T>
	inline void write(const T &value) {
		buf.sputn(reinterpret_cast<const char*>(&value), sizeof(value));
		size += sizeof(value);
	}

	template<typename T>
	inline T read() {
		char data[sizeof(T)];
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

