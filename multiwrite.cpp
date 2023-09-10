#include <iostream>

int main() {
	(std::cout << "Hello,").flush();
	(std::cout << ' ').flush();
	(std::cout << "World!").flush();
	std::cout << std::endl;
	std::cerr << "stderr.\n";
}
