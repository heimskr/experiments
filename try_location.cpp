#include "Timer.h"

#include <print>
#include <stdexcept>

constexpr int big = 50'000;

void __attribute__((__noinline__)) foo(int x, int y) {
	if (x == big && y == big) {
		throw std::runtime_error("Wow");
	}
}

int main() {
	{
		Timer timer{"Outer"};
		try {
			for (int x = 0; x <= big; ++x) {
				for (int y = 0; y <= big; ++y) {
					foo(x, y);
				}
			}
		} catch (const std::runtime_error &error) {
			std::println("Caught outer: {}", error.what());
		}
	}

	{
		Timer timer{"Inner"};
		for (int x = 0; x <= big; ++x) {
			for (int y = 0; y <= big; ++y) {
				try {
					foo(x, y);
				} catch (const std::runtime_error &error) {
					std::println("Caught inner: {}", error.what());
					break;
				}
			}
		}
	}

	{
		Timer timer{"Middle"};
		for (int x = 0; x <= big; ++x) {
			try {
				for (int y = 0; y <= big; ++y) {
					foo(x, y);
				}
			} catch (const std::runtime_error &error) {
				std::println("Caught middle: {}", error.what());
				break;
			}
		}
	}

	Timer::summary();
}
