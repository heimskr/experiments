#include <concepts>
#include <print>
#include <span>
#include <string>
#include <vector>

void printAll(std::span<const std::string> span) {
	for (const std::string &string: span) {
		std::println("- {{{}}}", string);
	}
}

struct Str {
	const char *str;

	Str(const char *str): str(str) {}

	template <typename Self>
	operator std::string(this Self &&self) {
		using S = decltype(self);
		std::println("{}Str{}{{\"{}\"}} -> std::string",
			std::is_const_v<std::remove_reference_t<S>>? "const " : "",
			std::is_rvalue_reference_v<S>? " &&" : std::is_lvalue_reference_v<S>? " &" : "",
			self.str);
		return self.str;
	}
};

template <typename From>
concept Stringable = std::constructible_from<std::string, From>;

void printAll(Stringable auto &&...strings) {
	// std::vector<std::string> vector{std::forward(strings)...}; // std::initializer_list kinda doesn't allow moves though

	std::vector<std::string> vector;
	vector.reserve(sizeof...(strings));
	(vector.emplace_back(std::forward<decltype(strings)>(strings)), ...);

	printAll(vector);
}

int main() {
	std::string bar = "bar";
	std::string_view baz = "baz";
	Str quux{"quux"};
	const Str exclamation{"!"};
	Str question{"?"};
	const Str ellipsis{"..."};
	printAll("foo", bar, baz, Str{"and"}, quux, exclamation, std::move(question), std::move(ellipsis));
}
