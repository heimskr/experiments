#include <print>

struct S {
	int x = 42;
	S() { std::println("S()"); }
	S(const S &) { std::println("S(const S &)"); }
	S(S &&) { std::println("S(S &&)"); }
	S & operator=(const S &) { std::println("S & S::operator=(const S &)"); return *this; }
	S & operator=(S &&) { std::println("S & S::operator=(S &&)"); return *this; }
};

int main() {
	S a, b;
	std::println("Going to iterate with auto &&.");
	for (auto &&s: {a, b}) {
		std::println("{}", s.x);
	}
	std::println("Going to iterate with auto &.");
	for (auto &s: {a, b}) {
		std::println("{}", s.x);
	}
}
