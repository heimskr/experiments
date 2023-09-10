// What happens if you call data() on an empty vector?
// On my setup (Arch Linux, 2023/9/9), g++ (13.2.1) and clang (16.0.6) both return a null pointer.

#include <iostream>
#include <vector>

int main() {
	std::vector<int> empty;
	std::cout << empty.data() << '\n';
	empty.push_back(42);
	std::cout << empty.data() << '\n';
}
