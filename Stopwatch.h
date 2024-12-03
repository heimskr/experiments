#pragma once

#include <chrono>
#include <utility>

class Stopwatch {
	public:
		std::chrono::system_clock::time_point last;

		Stopwatch():
			last(std::chrono::system_clock::now()) {}

		double lap() {
			auto now = std::chrono::system_clock::now();
			auto difference = now - std::exchange(last, now);
			return std::chrono::duration_cast<std::chrono::nanoseconds>(difference).count() / 1e9;
		}
};
