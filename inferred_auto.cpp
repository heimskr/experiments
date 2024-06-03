auto factorial(unsigned n) {
	if (n <= 1)
		return 1u; // Can't be just `1`
	return n * factorial(n - 1);
}

int main() {
	return factorial(4);
}
