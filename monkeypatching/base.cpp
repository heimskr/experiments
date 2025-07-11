#include "base.h"

#include <print>

void hej() {
	std::println("base hej");
}

long Foo::bar(int a, const std::string * volatile * volatile &&str, const unsigned long long &&, const int *) const volatile {
	std::println("base Foo::bar({}, {{{}}}): {}", a, **str, (int) x);
	return x + a;
}

void Foo::quux() const {
	std::println("base quux");
}
