#include "base.h"

#include <print>

void hej() {
	std::println("overridden hej");
}

long Foo::bar(int a, const std::string * volatile * volatile &&str, const unsigned long long &&, const int * const volatile) const volatile {
	std::println("overridden Foo::bar({}, {{{}}}): {}\n", a, **str, (int) x);
	return 666;
}
