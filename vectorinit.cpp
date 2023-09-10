// Direct initialization (parentheses) and list initialization (curly braces) can call different constructors. This file
// demonstrates that replacing all direct initializations with list initializations can have unintended consequences.

#include <cassert>
#include <iostream>
#include <vector>

int main() {
	std::vector<size_t> one(42ul, 64ul);
	std::vector<size_t> two{42ul, 64ul};
	std::cout << one.size() << '\n';
	std::cout << two.size() << '\n';
	assert(one.size() == two.size()); // Will fail!
}
