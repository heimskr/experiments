// Demonstrates that, given class A, class B that virtually inherits from A and class C that nonvirtually inherits from
// B, B's call to A's constructor will happen on a direct instantiation of B but it'll be skipped on a direct
// instantiation of C.

#include <iostream>

struct A {
	int x;
	A() = delete;
	A(int x_): x(x_) {
		std::cout << "  A::A(" << x_ << ")\n";
	};
};

struct B: virtual A {
	B(): A(16) {
		std::cout << "  B::B()\n";
	}
};

struct C: B {
	C(int x_): A(x_), B() { // B's call to A(16) doesn't happen!
		std::cout << "  C::C(" << x_ << ")\n";
	}
};

int main() {
	std::cout << "B():\n";
	B b;
	std::cout << "\nC(42):\n";
	C c(42);
	std::cout << "\nb.x: " << b.x << '\n';
	std::cout << "c.x: " << c.x << '\n';
}
