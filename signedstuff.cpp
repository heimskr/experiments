// This program demonstrates some signedness behavior.

#include <iostream>

signed int getFoo() {
	return -32;
}

int main() {
	signed int foo = getFoo();
	unsigned int bar = (unsigned int) foo;
	foo *= 2;
	bar *= 2;
	std::cout << foo << std::endl;
	std::cout << bar << std::endl;
}
