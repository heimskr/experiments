#include <print>
#include <numeric>
#include <vector>

int main() {
	std::vector<size_t> vector(32);
	std::ranges::iota(vector, 100);
	std::println("{}", vector);
}