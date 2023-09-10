// I don't remember the context, but this is a slightly trivial C program whose output varies.

#include <stdlib.h>

int bar(int x) {
	return x + rand() % 100;
}

int foo(int x) {
	do {
		x = bar(x);
	} while (10000 < x);
	return x / 2;
}

int main() {
	return foo(10) % 100;
}
