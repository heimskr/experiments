#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <map>
#include <random>
#include <unordered_map>

auto getNow() {
	return std::chrono::system_clock::now();
}

int main() {
	constexpr static uint_fast64_t SEED = 64;
	constexpr static uint64_t COUNT = 10'000'000;

	std::map<uint64_t, char> map;

	std::mt19937_64 rng(SEED);

	for (uint64_t i = 0; i < COUNT; ++i)
		map.emplace(rng(), i);

	std::unordered_map<uint64_t, char> hashmap(map.begin(), map.end());

	auto now = getNow();

	{
		uint64_t sum = 0;
		rng.seed(SEED);
		for (uint64_t i = 0; i < COUNT; ++i)
			sum += map[i];
		std::cout << std::format("Sum: {}\n", sum);
	}

	std::cout << std::format("Map sum: {} seconds\n", ((getNow() - now).count() / 1'000) / 1e6);


	now = getNow();

	{
		uint64_t sum = 0;
		rng.seed(SEED);
		for (uint64_t i = 0; i < COUNT; ++i)
			sum += hashmap[i];
		std::cout << std::format("Hashmap sum: {}\n", sum);
	}

	std::cout << std::format("Hashmap: {} seconds\n", ((getNow() - now).count() / 1'000) / 1e6);
}
