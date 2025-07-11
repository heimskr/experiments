#include <generator>
#include <print>
#include <string>

std::generator<int> generateSome(int n) {
	for (int i = 1; i <= n; ++i) {
		co_yield i;
	}
}

std::generator<int> generateMore(int n) {
	std::generator<int> gen = generateSome(n / 2);
	auto iter = gen.begin();
	for (int i = 0; i < n; ++i) {
		co_yield *iter;
		++iter;
	}
}

int main() {
	std::generator<int> some = generateMore(10);
	for (int i = 0; int x: some) {
		std::println("{} -> {}", i++, x);
	}
	std::println("Done.");
}