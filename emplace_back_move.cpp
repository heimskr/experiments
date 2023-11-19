// Demonstrates the behavior of emplace_back and push_back with lvalue vs. rvalue references.
// In this case, it's the same between emplace_back and push_back.

#include <iostream>
#include <vector>

struct A {
	int value{};

	A() { std::cout << "A::A()\n"; }
	A(int value_): value(value_) { std::cout << "A::A(int " << value_ << ")\n"; }
	A(const A &a): value(a.value) { std::cout << "A::A(const A &" << a.value << ")\n"; }
	A(A &&a): value(a.value) { std::cout << "A::A(A &&" << a.value << ")\n"; a.value = 0; }
	A & operator=(const A &a) {
		value = a.value;
		std::cout << "A::operator=(const A &" << a.value << ")\n";
		return *this;
	}
	A & operator=(A &&a) {
		value = a.value;
		std::cout << "A::operator=(A &&" << a.value << ")\n";
		a.value = 0;
		return *this;
	}
};

int main() {
	A a{42};
	std::vector<A> as;
	as.reserve(100);
	std::cout << "as.emplace_back(a):\n";
	as.emplace_back(a);
	std::cout << "a.value == 42: " << std::boolalpha << (a.value == 42) << '\n';
	std::cout << "as.emplace_back(std::move(a)):\n";
	as.emplace_back(std::move(a));
	std::cout << "a.value == 0: " << std::boolalpha << (a.value == 0) << '\n';
	a.value = 666;
	std::cout << "a.value = 666; as.push_back(a):\n";
	as.push_back(a);
	std::cout << "a.value == 666: " << std::boolalpha << (a.value == 666) << '\n';
	std::cout << "as.push_back(std::move(a)):\n";
	as.push_back(std::move(a));
	std::cout << "a.value == 0: " << std::boolalpha << (a.value == 0) << '\n';
}