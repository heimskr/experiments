#include <print>

void foo(float f) {
	std::println("float: {}", f);
}

void foo(double d) {
	std::println("double: {}", d);
}

int main() {
	foo(8.);
	foo(8.f);
}
