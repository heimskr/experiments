#include <memory>

struct A {};

int main() {
	std::shared_ptr a(new A);
}
