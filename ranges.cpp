#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
	std::vector<int> primes{2, 3, 5, 7, 11, 13, 17, 19};
	std::ranges::for_each(primes, [](int prime) {
		std::cout << (prime * 2) << '\n';
	});
}
