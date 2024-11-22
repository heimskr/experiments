#include <print>
#include <utility>

std::pair<int, int> getPair() {
	return {1, 2};
}

template <typename T>
std::pair<T, T> getPair() {
	const auto [x, y] = getPair();
	return {static_cast<T>(x), static_cast<T>(y)};
}

int main() {
	const auto [x, y] = getPair<double>();
	std::println("{} {}, {} {}", typeid(x).name(), x, typeid(y).name(), y);
}
