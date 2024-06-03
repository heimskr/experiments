#include <iostream>

enum class Grade{A, B, C, D, F};
enum class Language{Cpp, C, JavaScript};

template <typename T>
void print() {
	std::cout << static_cast<int>(T::C) << '\n';
}

int main() {
	print<Grade>();
	print<Language>();
}
