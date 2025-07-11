#include "base.h"

#include <print>

void hej() {
	std::println("base hej");
}

long Foo::bar(int a, const std::string * volatile * volatile &&str, const unsigned long long &&, const int * const volatile) const volatile {
	std::println("base Foo::bar({}, {{{}}}): {}\n", a, **str, (int) x);
	return x + a;
}
