#include <cassert>
#include <format>
#include <print>

struct Color {
	float red{};
	float green{};
	float blue{};
};

template <>
struct std::formatter<Color> {
	bool hash = false;

	constexpr auto parse(auto &ctx) {
		auto iter = ctx.begin();
		if (*iter == '#') {
			hash = true;
			++iter;
		}
		return iter;
	}

	auto format(const auto &color, auto &ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {} :: '{}')", color.red, color.green, color.blue, hash);
	}
};

int main() {
	Color color{1.0f, 0.5f, 0.0f};
	std::println("{:#}", color);
}