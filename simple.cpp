// This is a fairly trivial program that would be fairly easy to compile to assembly by hand.

int triple(int x) {
	return 2 * x + x;
}

unsigned sextuple(unsigned x) {
	return triple(static_cast<int>(x)) + 3 * x;
}

int main() {
	return sextuple(10);
}
