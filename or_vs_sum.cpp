// This program demonstrates that bitwise OR and addition produce the same result in some cases.

#include <iostream>

int main() {
	for (size_t i = 0; i <= 0b1111111; ++i) {
		const auto s = (i << 17) + (i << 10) + (i << 3);
		const auto o = (i << 17) | (i << 10) | (i << 3);
		if (s != o) {
			std::cout << "Mismatch (i = " << i << "): " << s << " != " << o << '\n';
		} else {
			std::cout << "i = " << i << '\n';
		}
	}
}
