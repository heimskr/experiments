#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

// Please include this header in at most one translation unit per program.
// Also, don't use this from multiple threads concurrently.

namespace {
	std::map<std::string, std::chrono::nanoseconds> timerTimes;
	std::map<std::string, size_t> timerCounts;
}

class Timer {
	public:
		std::chrono::system_clock::time_point start;
		std::string name;

		Timer(std::string_view name):
			start(std::chrono::system_clock::now()),
			name(name) {}

		template <typename... Args>
		Timer(std::format_string<Args...> format, Args &&...args):
			start(std::chrono::system_clock::now()),
			name(std::format(format, std::forward<Args>(args)...)) {}

		~Timer() {
			stop();
		}

		std::chrono::nanoseconds difference() const {
			return std::chrono::system_clock::now() - start;
		}

		void stop() {
			if (stopped) {
				return;
			}

			timerTimes[name] += difference();
			++timerCounts[name];
			stopped = true;
		}

		void restart() {
			start = std::chrono::system_clock::now();
			stopped = false;
		}

		static void summary(double threshold = 0.0) {
			if (!timerTimes.empty()) {
				std::println(std::cerr, "Timer summary:");

				std::vector<const std::string *> names;
				names.reserve(timerTimes.size());
				size_t max_length = 0;

				for (const auto &[name, nanos]: timerTimes) {
					if (nanos.count() / 1e9 < threshold) {
						continue;
					}

					names.push_back(&name);
					max_length = std::max(name.size(), max_length);
				}

				std::sort(names.begin(), names.end(), [](const std::string *left, const std::string *right) {
					return timerTimes.at(*left) > timerTimes.at(*right);
				});

				for (const std::string *name: names) {
					const double nanos = timerTimes.at(*name).count();
					std::print(std::cerr, "    \e[1m{}{}\e[22m took \e[32m{}\e[39m seconds", *name, std::string(max_length - name->size(), ' '), nanos / 1e9);
					const size_t count = timerCounts.at(*name);
					if (1 < count) {
						std::print(std::cerr, " (average: \e[33m{}\e[39m over \e[1m{}\e[22m instances)", nanos / count / 1e9, count);
					}
					std::println(std::cerr);
				}
			}
		}

		static void clear() {
			timerTimes.clear();
			timerCounts.clear();
		}

	private:
		bool stopped = false;
};
