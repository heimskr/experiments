#include <cassert>
#include <generator>
#include <print>
#include <string>

struct String {
	virtual ~String() = default;

	bool operator==(const String &other) const;
	virtual std::generator<std::string_view> iteratePieces(size_t pos, size_t n = std::string::npos) const = 0;
	virtual size_t getSize() const = 0;

	std::string resolve(size_t pos = 0, size_t n = std::string::npos);
};

struct Plain: String {
	std::string chars;

	Plain(std::string chars):
		chars(std::move(chars)) {}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;

	size_t getSize() const final {
		return chars.size();
	}
};

struct Substring: String {
	String *parent = nullptr;
	size_t offset = 0;
	size_t extent = 0;
	size_t size = 0;

	Substring(String &parent, size_t offset = 0, size_t extent = 0):
		parent(&parent),
		offset(offset),
		extent(extent) {
			if (parent.getSize() < offset) {
				throw std::out_of_range("Invalid substring");
			}

			size = std::min(parent.getSize() - offset, extent);
		}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;

	size_t getSize() const final { return size; }
};

struct Rope: String {
	String *fiber0 = nullptr;
	String *fiber1 = nullptr;
	String *fiber2 = nullptr;
	size_t size = 0;

	Rope(String &fiber0):
		fiber0(&fiber0),
		size(fiber0.getSize()) {}

	Rope(String &fiber0, String &fiber1):
		fiber0(&fiber0),
		fiber1(&fiber1),
		size(fiber0.getSize() + fiber1.getSize()) {}

	Rope(String &fiber0, String &fiber1, String &fiber2):
		fiber0(&fiber0),
		fiber1(&fiber1),
		fiber2(&fiber2),
		size(fiber0.getSize() + fiber1.getSize() + fiber2.getSize()) {}

	std::generator<std::string_view> iteratePieces(size_t pos, size_t n) const final;

	size_t getSize() const final { return size; }
};

bool String::operator==(const String &other) const {
	if (getSize() != other.getSize()) {
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

	for (;;) {
		if (iter1 == gen1.end() && iter2 == gen2.end()) {
			return true;
		}

		bool advance1 = true;
		bool advance2 = true;

		if (view1.size() < view2.size()) {
			if (view1 != view2.substr(0, view1.size())) {
				return false;
			}

			view2.remove_prefix(view1.size());
			advance2 = false;
		} else if (view1.size() > view2.size()) {
			if (view1.substr(0, view2.size()) != view2) {
				return false;
			}

			view1.remove_prefix(view2.size());
			advance1 = false;
		} else if (view1 != view2) {
			return false;
		}

		if (advance1) {
			do {
				if (++iter1 == gen1.end()) {
					break;
				}

				view1 = *iter1;
			} while (view1.empty());
		}

		if (advance2) {
			do {
				if (++iter2 == gen2.end()) {
					break;
				}

				view2 = *iter2;
			} while (view2.empty());
		}
	}
}

std::generator<std::string_view> Plain::iteratePieces(size_t pos, size_t n) const {
	co_yield std::string_view(chars).substr(pos, n);
}

std::generator<std::string_view> Substring::iteratePieces(size_t pos, size_t n) const {
	std::generator<std::string_view> subgenerator = parent->iteratePieces(offset);
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
		for (std::string_view view: fiber0->iteratePieces(0)) {
			co_yield view;
		}

		if (fiber1) {
			for (std::string_view view: fiber1->iteratePieces(0)) {
				co_yield view;
			}

			if (fiber2) {
				for (std::string_view view: fiber2->iteratePieces(0)) {
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
		if (n < view.size()) {
			co_yield view.substr(0, n);
			co_return;
		}

		if (n == view.size()) {
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
	assert(r2p.chars == "foobar_oob_foobar_oob");
	assert(r2 == r2p);
	assert(r2 != r2p2);

	std::println("Hooray.");
}
