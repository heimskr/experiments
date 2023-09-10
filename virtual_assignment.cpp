// This program demonstrates that operator= can be virtual, in case you're feeling a little quirky today.

#include <iostream>

struct Base {
	int x;

	Base(int x_):
		x(x_) {}

	virtual Base & operator=(const Base &) = default;
};

struct Derived: Base {
	int y;

	Derived(int x_, int y_):
		Base(x_), y(y_) {}

	Base & operator=(const Base &other) noexcept override {
		std::cout << "Base & operator=(const Base &other) noexcept override\n";
		return Base::operator=(other);
	}
};


int main() {
	Derived *a = new Derived(1, 2);
	Base *b = new Derived(3, 4);
	*b = *a;

	std::cout << "a = (" << a->x << ", " << a->y << "), "
	             "b = (" << b->x << ", " << dynamic_cast<Derived *>(b)->y << ")\n";
}
