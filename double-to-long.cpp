#include <cmath>
#include <print>

long cvt(double x) {
	return static_cast<long>(x);
}

int main() {
	for (double x: {-1.9, -1.5, -1.1, -1.0, 1.0, 1.1, 1.5, 1.9}) {
		std::println("{:4}: cast = {:2}, trunc = {:2}, floor = {:2}, ceil = {:2}", x, cvt(x), std::trunc(x), std::floor(x), std::ceil(x));
	}
}
