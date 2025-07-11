#include <print>

struct E {};

int main() {
	E e{};
	std::println("{}", static_cast<int>(*reinterpret_cast<char *>(&e)));
}
