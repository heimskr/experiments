// Demonstrates that moving from a value of an integral type leaves the original value unmodified.

#include <iostream>

int main() {
	int a = 42;
	int b = std::move(a);
	std::cout << "a = " << a << ", b = " << b << std::endl;
}
