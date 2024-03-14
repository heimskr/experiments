#include <iostream>
#include <string>

struct A {
	virtual std::string foo() const = 0;

	void a() {
		std::cout << "A: " << foo() << '\n';
	}
};

struct B {
	virtual std::string foo() const = 0;

	void b() {
		std::cerr << "B: " << foo() << '\n';
	}
};

struct C: A, B {
	std::string foo() const override { return "C"; }
};

int main() {
	C c;
	c.a();
	c.b();
}
