#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

int main() {
	nlohmann::json one{1, 2, 3};
	nlohmann::json two{4, 5, 6};
	one += two;
	std::cout << one.dump() << '\n';
}
