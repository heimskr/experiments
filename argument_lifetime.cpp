// Demonstrates that temporaries constructed for a function argument aren't destroyed until after the function call
// completes.

#include <iostream>

struct B;

struct A {
	int x;

	A() = delete;

	explicit A(int x_): x(x_) {
		std::cout << "A::A(" << x_ << ") → " << this << std::endl;
	}

	~A() {
		std::cout << "A::~A(" << this << ")" << std::endl;
	}

	operator B() const;
};

struct B {
	const A *a;

	B() = delete;

	explicit B(const A &a_) {
		std::cout << "B::B(" << &a_ << ") → " << this << std::endl;
		a = &a_;
	}

	~B() {
		std::cout << "B::~B(" << this << ')' << std::endl;
	}
};

A::operator B() const {
	return B(*this);
}

void foo(const B &b) {
	std::cout << "foo: b.a->x = " << b.a->x << std::endl;
}

int main() {
	foo(A(42));
	std::cout << std::endl;
	foo(B(A(64)));
}
