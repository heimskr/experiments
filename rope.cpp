#include <cassert>
#include <chrono>
#include <generator>
#include <print>
#include <random>
#include <string>
#include <vector>

class String {
	public:
		size_t size = 0;

		virtual ~String() = default;

		bool operator==(const String &other) const;
		virtual std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const = 0;

		std::string resolve(size_t pos = 0, size_t n = std::string::npos);

	protected:
		String(size_t size):
			size(size) {}
};

struct Plain: String {
	std::string chars;

	Plain(std::string chars):
		String(chars.size()),
		chars(std::move(chars)) {}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;
};

struct Substring: String {
	String *parent = nullptr;
	size_t offset = 0;
	size_t extent = 0;

	Substring(String &parent, size_t offset = 0, size_t extent = 0):
		String(0),
		parent(&parent),
		offset(offset),
		extent(extent) {
			if (parent.size < offset) {
				throw std::out_of_range("Invalid substring");
			}

			size = std::min(parent.size - offset, extent);
		}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;
};

struct Rope: String {
	String *fiber0 = nullptr;
	String *fiber1 = nullptr;
	String *fiber2 = nullptr;

	Rope(String &fiber0):
		String(fiber0.size),
		fiber0(&fiber0) {}

	Rope(String &fiber0, String &fiber1):
		String(fiber0.size + fiber1.size),
		fiber0(&fiber0),
		fiber1(&fiber1) {}

	Rope(String &fiber0, String &fiber1, String &fiber2):
		String(fiber0.size + fiber1.size + fiber2.size),
		fiber0(&fiber0),
		fiber1(&fiber1),
		fiber2(&fiber2) {}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;
};

bool String::operator==(const String &other) const {
	if (this == &other) {
		return true;
	}

	if (size != other.size) {
		return false;
	}

	std::generator<std::string_view> gen1 = iteratePieces(0, std::string::npos);
	std::generator<std::string_view> gen2 = other.iteratePieces(0, std::string::npos);

	auto iter1 = gen1.begin();
	auto iter2 = gen2.begin();

	if (iter1 == gen1.end() || iter2 == gen2.end()) {
		assert(iter1 == gen1.end());
		assert(iter2 == gen2.end());
		return false;
	}

	std::string_view view1 = *iter1;
	std::string_view view2 = *iter2;

	auto advance = [](auto &iter, auto &gen, auto &view) {
		do {
			if (++iter == gen.end()) {
				break;
			}

			view = *iter;
		} while (view.empty());
	};

	for (;;) {
		if (iter1 == gen1.end() && iter2 == gen2.end()) {
			return true;
		}

		if (view1.size() < view2.size()) {
			if (view1 != view2.substr(0, view1.size())) {
				return false;
			}

			view2.remove_prefix(view1.size());
			advance(iter1, gen1, view1);
		} else if (view1.size() > view2.size()) {
			if (view1.substr(0, view2.size()) != view2) {
				return false;
			}

			view1.remove_prefix(view2.size());
			advance(iter2, gen2, view2);
		} else if (view1 == view2) {
			advance(iter1, gen1, view1);
			advance(iter2, gen2, view2);
		} else {
			return false;
		}
	}
}

std::generator<std::string_view> Plain::iteratePieces(size_t pos, size_t n) const {
	co_yield std::string_view(chars).substr(pos, n);
}

std::generator<std::string_view> Substring::iteratePieces(size_t pos, size_t n) const {
	std::generator<std::string_view> subgenerator = parent->iteratePieces(offset, std::string::npos);
	std::string_view view;

	auto iter = subgenerator.begin();

	if (iter == subgenerator.end()) {
		co_return;
	}

	if (pos != 0) {
		for (size_t to_skip = pos; iter != subgenerator.end(); ++iter) {
			std::string_view next_view = *iter;

			if (to_skip == 0) {
				view = next_view;
				break;
			}

			if (next_view.size() < to_skip) {
				to_skip -= next_view.size();
				continue;
			}

			if (next_view.size() == to_skip) {
				to_skip = 0;
				continue;
			}

			view = next_view.substr(next_view.size() - to_skip);
			break;
		}

		if (iter == subgenerator.end()) {
			co_return;
		}
	} else {
		view = *iter;
	}

	size_t remaining = std::min(extent, n);


	for (;;) {
		if (remaining < view.size()) {
			co_yield view.substr(0, remaining);
			co_return;
		}

		if (remaining == view.size()) {
			co_yield view;
			co_return;
		}

		co_yield view;
		remaining -= view.size();
		if (++iter == subgenerator.end()) {
			co_return;
		}

		view = *iter;
	}
}

std::generator<std::string_view> Rope::iteratePieces(size_t pos, size_t n) const {
	assert(fiber0 != nullptr);

	auto generator = [this] -> std::generator<std::string_view> {
		for (std::string_view view: fiber0->iteratePieces(0, std::string::npos)) {
			co_yield view;
		}

		if (fiber1) {
			for (std::string_view view: fiber1->iteratePieces(0, std::string::npos)) {
				co_yield view;
			}

			if (fiber2) {
				for (std::string_view view: fiber2->iteratePieces(0, std::string::npos)) {
					co_yield view;
				}
			}
		} else if (fiber2) {
			throw std::runtime_error("Gaps not allowed in rope strings");
		}
	}();

	auto iter = generator.begin();
	std::string_view view;

	if (iter == generator.end()) {
		co_return;
	}

	if (pos != 0) {
		for (size_t to_skip = pos; iter != generator.end(); ++iter) {
			std::string_view next_view = *iter;

			if (to_skip == 0) {
				view = next_view;
				break;
			}

			if (next_view.size() < to_skip) {
				to_skip -= next_view.size();
				continue;
			}

			if (next_view.size() == to_skip) {
				to_skip = 0;
				continue;
			}

			view = next_view.substr(next_view.size() - to_skip);
			break;
		}

		if (iter == generator.end()) {
			co_return;
		}
	} else {
		view = *iter;
	}

	size_t remaining = n;

	for (;;) {
		if (remaining < view.size()) {
			co_yield view.substr(0, remaining);
			co_return;
		}

		if (remaining == view.size()) {
			co_yield view;
			co_return;
		}

		co_yield view;
		remaining -= view.size();
		if (++iter == generator.end()) {
			co_return;
		}

		view = *iter;
	}
}

std::string String::resolve(size_t pos, size_t n) {
	std::string out;

	for (std::string_view view: iteratePieces(pos, n)) {
		out += view;
	}

	return out;
}

class Timer {
	public:
		std::chrono::nanoseconds stop() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start);
		}

	private:
		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
};

void timing() {
	std::chrono::nanoseconds with_smart_compare{};
	std::chrono::nanoseconds with_resolve{};

	constexpr size_t bound = 100'000;
	constexpr size_t iters = 10'000;

	std::default_random_engine rng{1248163264128256};
	std::uniform_int_distribution<char> printable{'!', '~'};
	std::uniform_int_distribution<size_t> length{0uz, bound};

	size_t smart_equal = 0;
	size_t resolve_equal = 0;

	Timer outer_timer;

	for (size_t i = 0; i < iters; ++i) {
		std::vector<std::string> strings(3);

		for (std::string &string: strings) {
			string = std::string(length(rng), '\0');
			for (char &ch: string) {
				ch = printable(rng);
			}
		}

		Plain p0{strings[0]};
		Plain p1{strings[1]};
		Plain p2{strings[2]};
		Plain p3{strings[2]};
		Substring s1{p1, std::min(strings[1].size(), 20uz), std::max(strings[1].size(), 20uz) - 20};
		Rope r1{p0, s1, p2};
		Rope r2{p0, s1, p3};

		{
			Timer timer;
			smart_equal += r1 == r2;
			with_smart_compare += timer.stop();
		}

		{
			Timer timer;
			resolve_equal += r1.resolve() == r2.resolve();
			with_resolve += timer.stop();
		}
	}

	std::println("Total time for {} iterations and upper length bound of {}: {:.3f} ms", iters, bound, outer_timer.stop().count() / 1'000'000.0);
	std::println("Smart compare time:   {:.3f} ms", with_smart_compare.count() / 1'000'000.0);
	std::println("Resolve compare time: {:.3f} ms", with_resolve.count() / 1'000'000.0);
	std::println("Reduction: {:.3f}%", 100 - 100 * static_cast<double>(with_smart_compare.count()) / with_resolve.count());

	assert(smart_equal == iters);
	assert(resolve_equal == iters);
}

int main() {
	Plain p1{"foobar"};
	Plain p2{"_"};
	Substring s1{p1, 1, 3};
	Rope r1{p1, p2, s1};
	Rope r2{r1, p2, r1};
	Plain r2p{r2.resolve()};
	Plain r2p2{r2.resolve()};
	r2p2.chars.back() = '!';

	assert(r1.resolve() == "foobar_oob");
	assert(p1.resolve() == "foobar");
	assert(s1.resolve() == "oob");
	assert(p1 != p2);
	assert(r2p.chars == "foobar_oob_foobar_oob");
	assert(r2 == r2p);
	assert(r2 != r2p2);

	timing();

	std::println("Done.");
}
