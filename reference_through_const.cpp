// Demonstrates that non-const reference fields of a class don't become const references when accessing the class
// through a const reference or when the class itself is const.

#include <iostream>

struct Refs {
	int &a;
	Refs(int &a_): a(a_) {}
};

struct Ptrs {
	int *a;
	Ptrs(int *a_): a(a_) {}
};

int main() {
	int n = 0;

	Ptrs ptrs{&n};
	const Ptrs &cptrs = ptrs;
	std::cout << ++*ptrs.a << std::endl;
	std::cout << ++*cptrs.a << std::endl;

	Refs refs{n};
	const Refs &crefs = refs;
	std::cout << ++refs.a << std::endl;
	std::cout << ++crefs.a << std::endl;

	const Refs crefs2{n};
	std::cout << ++crefs2.a << std::endl;
}
