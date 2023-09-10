// This program demonstrates that you can initialize const references with rvalues (prvalues and xvalues).
// It also tests some weirdness with prefix operator++ on an xvalue returning an lvalue to itself.

#include <iostream>
#include <list>

int main() {
	std::list<int> list {1, 2, 3};

	const auto &ref = list.begin();
	const auto &num = 42;

	std::cout << &++const_cast<decltype(list)::iterator &>(ref) << '\n';
	std::cout << &++list.begin() << '\n';
}
