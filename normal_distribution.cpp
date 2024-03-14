#include <format>
#include <iostream>
#include <random>

float mutate(float strength) {
	const float stddev = strength / 6.f;
	const static float base = 1.f / std::sqrt(2.f * M_PIf);

	std::random_device rng;

	float normal = std::normal_distribution<float>(0.f, stddev)(rng);
	normal *= strength / base;

	return normal;
}

int main() {
	for (float strength = 0.f; strength <= 1.f; strength += 0.125f) {
		for (int i = 1; i <= 10; ++i) {
			std::cout << std::format("{}: {:2} → {}\n", strength, i, mutate(strength));
		}
	}
}
