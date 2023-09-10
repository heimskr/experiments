// This program calculates the number of possible TCP window sizes.
// Output: 524288 (which happens to be 2**19).

#include <iostream>
#include <set>

int main() {
	std::set<size_t> set;

	for (int shift = 0; shift <= 14; ++shift)
		for (int value = 0; value < 65536; ++value)
			set.insert(value << shift);

	std::cout << set.size() << '\n';
}
