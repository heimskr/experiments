#include <chrono>
#include <print>
#include <random>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

struct Pair {
	int32_t x = 0;
	int32_t y = 0;

	Pair() = default;
	Pair(int32_t x, int32_t y):
		x(x), y(y) {}

	inline bool operator==(const Pair &other) const {
		return this == &other || (x == other.x && y == other.y);
	}

	auto operator<=>(const Pair &) const = default;

	Pair operator+(const Pair &) const;
	Pair operator-(const Pair &) const;
	Pair & operator+=(const Pair &);
	Pair & operator-=(const Pair &);
};

Pair Pair::operator+(const Pair &other) const {
	return {x + other.x, y + other.y};
}

Pair Pair::operator-(const Pair &other) const {
	return {x - other.x, y - other.y};
}

Pair & Pair::operator+=(const Pair &other) {
	x += other.x;
	y += other.y;
	return *this;
}

Pair & Pair::operator-=(const Pair &other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

template <>
struct std::hash<Pair> {
	size_t operator()(const auto &pair) const {
		return std::hash<uint64_t>{}((static_cast<uint64_t>(pair.x) << 32) | static_cast<uint64_t>(pair.y));
	}
};

auto getTime() {
	return std::chrono::system_clock::now();
}

auto getDiff(auto duration) {
	return std::format("{:.3f} ms", std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e6);
}

template <template <typename...> typename C>
void test(const std::vector<Pair> &pair_vec, std::string_view name) {
	const auto test_start = getTime();
	auto start = test_start;
	decltype(start) end;
	{
		C<Pair> set(pair_vec.begin(), pair_vec.end());
		end = getTime();
		std::println("{} construct: {}", name, getDiff(end - std::exchange(start, end)));
		Pair sum{};
		for (const auto &pair: set) {
			sum += pair;
		}
		end = getTime();
		std::println("{} iterate (const &) -> ({}, {}): {}", name, sum.x, sum.y, getDiff(end - std::exchange(start, end)));
		sum = Pair{};
		for (auto pair: set) {
			sum += pair;
		}
		end = getTime();
		std::println("{} iterate (copy): -> ({}, {}): {}", name, sum.x, sum.y, getDiff(end - std::exchange(start, end)));
	}
	end = getTime();
	std::println("{} destroy: {}", name, getDiff(end - std::exchange(start, end)));
	std::println("{} total: {}", name, getDiff(end - test_start));
}

int main() {
	std::vector<Pair> pair_vec;
	std::default_random_engine rng(42);
	auto start = getTime();
	for (size_t i = 0; i < 1'000'000; ++i) {
		pair_vec.emplace_back(rng(), rng());
	}
	std::println("std::vector construct (size = {}): {}", pair_vec.size(), getDiff(getTime() - start));

	test<std::set>(pair_vec, "std::set");
	test<std::unordered_set>(pair_vec, "std::unordered_set");
}
