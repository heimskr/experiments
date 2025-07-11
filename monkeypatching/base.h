#pragma once

#include <string>

void hej();

struct Foo {
	int x{};

	long bar(int, const std::string * volatile * volatile &&, const unsigned long long &&, const int * const volatile) const volatile;

	template <typename A, typename B>
	void baz(B const &, A &&) {}
};
