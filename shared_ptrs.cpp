#include <memory>
#include <print>
#include <string>

int main() {
	std::shared_ptr<std::string> sptr = std::make_shared<std::string>("Hello");
	std::println("Initial use count: {}", sptr.use_count());

	{
		std::shared_ptr<std::string> sptr_copy = sptr;
		std::println("Use count after copy: {} (also {})", sptr.use_count(), sptr_copy.use_count());
	}

	std::println("Use count after copy expires: {}", sptr.use_count());
}
