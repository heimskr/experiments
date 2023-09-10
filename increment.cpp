// I don't remember the exact context, but I believe this was to prove some point about prefix vs. postfix increment to
// someone on Discord.

#include <iostream>

struct Thing {
	int i;
	Thing(int i_): i(i_) {}
	int inc(const char *where) {
		std::cout << "\e[1m" << where << "\e[22m: " << i << " -> " << (i + 1) << '\n';
		return i++;
	}
	operator int() const { return i; }
};

int main() {
	Thing i(1);
	for (; i.inc("i++ <= 1") <= 1; i.inc("end 1"))
		for (i.inc("init"); i.inc("i++ <= 6") <= 6; i.inc("end 2"))
			i.inc("inner");
	std::cout << i << '\n';
}
