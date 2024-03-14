#include <iostream>
#include <type_traits>

template <typename T>
concept ReturnsVoid = requires(T t) {
	requires std::is_same_v<decltype(t()), void>;
};

template <typename T, typename R, typename... Args>
concept Returns = requires(T t, Args &&...args) {
	requires std::is_same_v<decltype(t(std::forward<Args>(args)...)), R>;
};

int main() {
	auto returns_void = []() -> void {};
	auto returns_bool = [] { return false; };
	auto void_args = [](int a, char *&&b) -> void {};

	std::cout << ReturnsVoid<decltype(returns_void)> << '\n';
	std::cout << ReturnsVoid<decltype(returns_bool)> << '\n';
	std::cout << Returns<decltype(void_args), void> << '\n';
}
