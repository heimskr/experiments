#include <cmath>
#include <iostream>
#include <print>

int main() {
	double d;
	while (std::cin >> d) {
		double intpart{};
		double fractional = std::modf(d, &intpart);
		std::println("{} -> {} + {}", d, intpart, fractional);
	}
}