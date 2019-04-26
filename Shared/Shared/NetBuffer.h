#pragma once

#include <sstream>

class NetBuffer {
	private:
	std::stringbuf buf;
	public:
	template<typename T>
	inline void write(const T &value) {
		buf.sputn(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	template<typename T>
	inline T read() {
		char data[sizeof(T)];
		buf.sgetn(data, sizeof(T));
		return *reinterpret_cast<T*>(data);
	}
};

