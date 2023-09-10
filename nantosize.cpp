// This program demonstrates that the only consistent NaN value is from the nan(const char *) function and that INFINITY
// also varies across program invocations.

#include <cmath>
#include <iostream>

int main() {
	std::cout << static_cast<size_t>(NAN) << '\n';
	std::cout << static_cast<size_t>(NAN) << '\n';
	std::cout << static_cast<size_t>(NAN) << '\n';
	std::cout << static_cast<size_t>(NAN) << '\n';
	std::cout << static_cast<size_t>(nan("")) << '\n';
	std::cout << static_cast<size_t>(INFINITY) << '\n';
	std::cout << static_cast<size_t>(INFINITY) << '\n';
	std::cout << static_cast<size_t>(INFINITY) << '\n';
	std::cout << static_cast<size_t>(INFINITY) << '\n';
}
