#pragma once

#include <sstream>

class NetBuffer {
	private:
	std::stringbuf buf;
	public:
	NetBuffer() {
		//write<int32_t>(0);
	}

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

	void finish() {
		auto size = buf.str().size();
		std::cout << size << std::endl;
		buf.pubseekpos(0);
		write<int32_t>(size);
	}

	int32_t getSize() {
		return read<int32_t>();
	}
};

