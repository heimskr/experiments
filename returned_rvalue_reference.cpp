#include <iostream>
#include <print>
#include <string>

struct A {
	int x{};

	A() {
		std::println("A::A()");
	}

	A(int x): x(x) {
		std::println("A::A(int {})", x);
	}

	A(const A &a): x(a.x) {
		std::println("A::A(const A &)");
	}

	A(A &&a): x(a.x) {
		a.x = 0;
		std::println("A::A(A &&)");
	}

	A & operator=(const A &a) {
		x = a.x;
		std::println("A::operator=(const A &)");
		return *this;
	}

	A & operator=(A &&a) {
		x = a.x;
		a.x = 0;
		std::println("A::operator=(A &&)");
		return *this;
	}
};

A && foo(A &a) {
	return std::move(a);
}

A bar() {
	return A{42};
}

A baz() {
	A a = bar();
	return foo(a);
}

int main() {
	A a = baz();
	return a.x;
}
