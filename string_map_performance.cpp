#include "Stopwatch.h"
#include "Timer.h"

#include <cstdint>
#include <format>
#include <iostream>
#include <map>
#include <print>
#include <random>
#include <string>
#include <unordered_map>

std::string randomString(std::mt19937_64 &rng, size_t length, std::string_view prefix) {
	std::string out(prefix);

	if (length < out.size()) {
		out.erase(length);
		return out;
	}

	for (size_t i = out.size(); i < length; ++i) {
		out.push_back(static_cast<char>(rng()));
	}

	return out;
}

std::string randomIdentifier(std::mt19937_64 &rng, size_t length) {
	std::string out = "base:";
	out.push_back(static_cast<char>(rng()));
	out.push_back('/');

	for (size_t i = out.size(); i < length; ++i) {
		out.push_back(static_cast<char>(rng()));
	}

	return out;
}

inline std::string generateString(std::mt19937_64 &rng, size_t length, std::string_view prefix) {
	return randomString(rng, length, prefix);
	// return randomIdentifier(rng, length);
}

template <template <typename...> typename M, size_t Length, bool Prefixed, uint64_t Seed = 64>
uint64_t test(std::string_view name, uint64_t count) {
	M<std::string, std::string> map;
	std::mt19937_64 rng(Seed);

	for (uint64_t i = 0; i < count; ++i) {
		std::string string = generateString(rng, Length, Prefixed? "" : std::string_view{});
		Timer timer{"{}: emplace", name};
		map[string] = std::move(string);
	}

	volatile uint64_t sum = 0;
	Timer timer{"{}: iterate", name};
	for (const auto &[key, value]: map) {
		sum += value.size();
	}

	return sum;
}

int main() {
	constexpr uint_fast64_t SEED = 64;
	constexpr uint64_t COUNT_MAX = 1'000'000;
	constexpr size_t LENGTH = 32;

	Stopwatch stopwatch;
	for (uint64_t count = 1; count <= COUNT_MAX; count *= 10) {
		std::println("0/4. Count is \e[1m{}\e[22m.", count);
		test<std::map, LENGTH, true, SEED>("std::map (prefixed)", count);
		std::println("1/4. {} seconds.", stopwatch.lap());
		test<std::unordered_map, LENGTH, true, SEED>("std::unordered_map (prefixed)", count);
		std::println("2/4. {} seconds.", stopwatch.lap());
		test<std::map, LENGTH, false, SEED>("std::map", count);
		std::println("3/4. {} seconds.", stopwatch.lap());
		test<std::unordered_map, LENGTH, false, SEED>("std::unordered_map", count);
		std::println("4/4. {} seconds.\n", stopwatch.lap());
		Timer::summary();
		Timer::clear();
		std::println();
	}
}
