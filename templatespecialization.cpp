// This program demonstrates some simple template specialization.

#include <iostream>
#include <string>

template <typename T>
std::string stringify(const T &value) { // Default
	// Uses operator std::string() if available, or potentially a constructor of std::string
	return "DEFAULT: " + std::string(value);
}

template <>
std::string stringify(const int &value) { // Has to be a const ref here to match the above signature
	return "INT: " + std::to_string(value);
}

template <>
std::string stringify(const std::string &value) {
	return "STRING: " + value;
}

template <>
std::string stringify(const char &value) {
	return std::string("CHAR: ") + value;
}

struct S {
	S() = default;
	operator std::string() const { return "hello from S::operator std::string()"; }
};

int main() {
	S s;
	int i = 32;
	std::string str = "hello";
	std::cout << stringify(s) << '\n';
	std::cout << stringify(i) << '\n';
	std::cout << stringify(str) << '\n';
	std::cout << stringify('!') << '\n';
}
