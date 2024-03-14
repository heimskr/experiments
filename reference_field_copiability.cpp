#include <iostream>

struct Foo {
	int &num;
	Foo(int &num_): num(num_) {}
	Foo(const Foo &) = default;
	Foo(Foo &&) = default;
	Foo & operator=(const Foo &) = default;
	Foo & operator=(Foo &&) = default;
};

int main() {
	int x = 42;
	Foo one(x);
	Foo two(one);
	Foo three(std::move(two));
	++one.num;
	++two.num;
	++three.num;
	// two = std::move(one);
	// three = one;
	std::cout << x << std::endl;
}
