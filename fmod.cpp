#include <cmath>
#include <format>
#include <iostream>

int main() {
	for (float f = -2.0f; f <= 2.0f; f += 0.125f) {
		float mod = std::fmod(f, 1.f);
		std::cout << std::format("{:+6f} -> {:+.3f} : {}\n", f, mod, std::signbit(mod));
	}
}
