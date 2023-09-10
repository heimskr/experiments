// This program demonstrates how to use the `mutable` keyword for class/struct fields.

struct Foo {
	mutable int x{};

	const Foo & operator++() const {
		++x;
		return *this;
	}
};

int main() {
	Foo foo;
	const Foo &ref = foo;
	ref.x = 41;
	return (++ref).x;
}
