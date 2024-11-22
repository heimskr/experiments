#include <print>

struct S {
	int x;

	auto get_fn(this auto self) {
		return [this] { // no
			std::println("x = {}", x);
		};
	}
};

int main() {
	S{}.get_fn()();
}
