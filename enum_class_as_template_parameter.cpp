#include <iostream>

enum class Char: char {
	Exclamation = '!',
	Period = '.',
};

template <Char C>
void print() {
	std::cout << char(C) << '\n';
}

int main() {
	print<Char::Exclamation>();
}
