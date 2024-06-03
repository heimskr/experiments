#include <nlohmann/json.hpp>

#include <iostream>

int main() {
	std::cout << nlohmann::json{{"foo", std::vector<int>{}}}.dump() << '\n';
	std::cout << nlohmann::json{{"foo", nlohmann::json{}}}.dump() << '\n';
}
