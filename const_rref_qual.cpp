#include <print>

struct S {
	int foo() const && {
		return 42;
	}
};

int main() {
	std::println("{}", S{}.foo());
}
