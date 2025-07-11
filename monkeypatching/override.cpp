#include "base.h"

#include <print>

void hej() {
	std::println("overridden hej");
}

long Foo::bar(int a, const std::string * volatile * volatile &&str, const unsigned long long &&, const int *) const volatile {
	std::println("overridden Foo::bar({}, {{{}}}): {}", a, **str, (int) x);
	return 666;
}

void Foo::quux() const {
	std::println("overridden quux");
}
