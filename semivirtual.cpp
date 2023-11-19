// Demonstrates that given a diamond inheritance scenario with one inheritance from the base class being
// virtual and the other not, there will be two different copies of the base class in the most derived class.

#include <iostream>

struct A {
	int counter = 0;
};

struct B: virtual A {
	void incB(int n = 1) { counter += n; }
	int getB() const { return counter; }
};

struct C: A {
	void incC(int n = 1) { counter += n; }
	int getC() const { return counter; }
};

struct D: B, C {};

int main() {
	D d;
	d.incB(10);
	d.incC(5);
	std::cout << d.getB() << ' ' << d.getC() << '\n';
}
