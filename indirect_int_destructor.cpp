#include <print>

template <typename T>
void destroy(T t) {
	t.~T();
}

struct S {
	~S() { std::println("~S()"); }
};

int main() {
	destroy(S{});
	destroy(42);
}
