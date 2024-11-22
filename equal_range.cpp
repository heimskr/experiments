#include <map>
#include <print>
#include <string>

int main() {
	std::multimap<int, std::string> map{{1, "one"}, {2, "two"}, {2, "zwei"}, {4, "four"}};

	for (int i = 0; i <= 5; ++i) {
		auto [iter, end] = map.equal_range(i);
		std::println("i = {}:", i);
		std::println("  iter == end: {}", iter == end);
		std::println("  iter == map.end(): {}", iter == map.end());
	}
}