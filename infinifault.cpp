// This is just computer torture.

#include <csignal>
#include <print>

int main() {
	signal(SIGSEGV, +[](int) {
		// std::println("Segfault!");
	});
	*(volatile char *) nullptr = 42;
}
