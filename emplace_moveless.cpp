// Demonstrates that emplace_back works differently from push_back. Intentionally doesn't compile.

#include <iostream>
#include <vector>

struct A {
	int value{};

	A() { std::cout << "A::A()\n"; }
	A(int value_): value(value_) { std::cout << "A::A(int)\n"; }
	A(const A &a): value(a.value) { std::cout << "A::A(const A &)\n"; }
	A(A &&a) = delete;
	A & operator=(const A &a) { value = a.value; std::cout << "A::operator=(const A &)\n"; return *this; }
	A & operator=(A &&a) = delete;
};

int main() {
	std::vector<A> as;
	as.reserve(100);
	std::cout << "as.emplace_back(42):\n";
	as.emplace_back(42); // Valid
	std::cout << "as.push_back(A(42)):\n";
	as.push_back(A(42)); // Error!
}
