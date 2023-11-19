// Demonstrates that a const char * overload will be preferred over const std::string & one when given nullptr.
// Someone told me it would be const std::string & and I had to check for myself.

#include <iostream>
#include <string>

void foo(const char *str) {
	std::cout << "const char *: " << str << '\n';
}

void foo(const std::string &str) {
	std::cout << "std::string: " << str << '\n';
}

int main() {
	foo(nullptr);
}
