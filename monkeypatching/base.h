#pragma once

#include <string>

void hej();

struct Foo {
	int x{};

	Foo(int x): x(x) {}

	long bar(int, const std::string * volatile * volatile &&, const unsigned long long &&, const int *) const volatile;

	template <typename A, typename B>
	void baz(B const &, A &&) {}

	virtual void quux() const;
};
