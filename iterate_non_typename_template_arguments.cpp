#include <iostream>

enum class Enum {A, B, C};

template <Enum... E>
void iterate() {
	for (Enum e: {E...})
		std::cout << char(char(e) + 'A');
	std::cout << '\n';
}

int main() {
	iterate<Enum::B, Enum::A, Enum::C>();
}
