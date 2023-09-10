// This program demonstrates exception polymorphism.

#include <iostream>

struct A {
	virtual ~A() = default; // to make A polymorphic
};

struct B: A {};

int main() {
	try {
		throw B();
	} catch (A &a) {
		std::cout << typeid(a).name() << '\n';
	}
}
