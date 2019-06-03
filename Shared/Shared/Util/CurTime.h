#pragma once

#include <chrono>

static inline float curTime() {
	using namespace std::chrono;
	constexpr auto seconds = static_cast<float>(milliseconds::period::den);

	auto now = time_point_cast<milliseconds>(high_resolution_clock::now());
	return now.time_since_epoch().count() / seconds;
}

