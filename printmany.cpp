#include <cstdint>
#include <iostream>

template <typename... Args>
void printMany(Args &&...args) {
	(std::cout << ... << args) << '\n';
}

void printInts(std::integral auto... args) {
	(std::cout << ... << args) << '\n';
}

int main() {
	printMany(1, "foo", 3.0);
	printInts(1, 2, 3, uint8_t('a'));
}
