#include <array>
#include <cassert>
#include <chrono>
#include <generator>
#include <memory>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <vector>

class IteratorImpl {
	public:
		enum class Type {Plain, Substring, Rope};
		virtual ~IteratorImpl() = default;
		virtual Type getType() const = 0;
		virtual void next() = 0;
		virtual std::unique_ptr<IteratorImpl> clone() = 0;
		virtual std::string_view operator*() = 0;
		virtual bool atEnd() const = 0;
		virtual bool operator==(const IteratorImpl &) const = 0;
};

class StringIterator {
	public:
		std::unique_ptr<IteratorImpl> impl;

		explicit StringIterator(std::unique_ptr<IteratorImpl> impl):
			impl(std::move(impl)) {}

		StringIterator & operator++() {
			impl->next();
			return *this;
		}

		StringIterator operator++(int) {
			auto clone = impl->clone();
			impl->next();
			return StringIterator{std::move(clone)};
		}

		std::string_view operator*() const {
			return **impl;
		}

		bool operator==(const StringIterator &other) const {
			return this == &other || *impl == *other.impl;
		}
};

class String {
	public:
		size_t size = 0;

		virtual ~String() = default;

		bool operator==(const String &other) const;

		virtual StringIterator begin() const = 0;
		virtual StringIterator end() const = 0;

		std::string resolve(size_t pos = 0, size_t n = std::string::npos);

	protected:
		String(size_t size):
			size(size) {}
};

class Plain: public String {
	public:
		std::string chars;

		Plain(std::string chars):
			String(chars.size()),
			chars(std::move(chars)) {}

		StringIterator begin() const final {
			return StringIterator{std::make_unique<Iterator>(*this, false)};
		}

		StringIterator end() const final {
			return StringIterator{std::make_unique<Iterator>(*this, true)};
		}

	private:
		class Iterator: public IteratorImpl {
			public:
				Iterator(const Plain &owner, bool isEnd):
					owner(owner),
					isEnd(isEnd) {}

				Type getType() const final { return Type::Plain; }

				void next() final {
					assert(!isEnd);
					isEnd = true;
				}

				std::unique_ptr<IteratorImpl> clone() final {
					return std::make_unique<Iterator>(owner, isEnd);
				}

				std::string_view operator*() final {
					assert(!isEnd);
					return owner.chars;
				}

				bool atEnd() const final {
					return isEnd;
				}

				bool operator==(const IteratorImpl &other) const final {
					if (this == &other) {
						return true;
					}

					if (other.getType() != Type::Plain) {
						return false;
					}

					auto &plain = static_cast<const Iterator &>(other);
					return &owner == &plain.owner && isEnd == plain.isEnd;
				}

			private:
				const Plain &owner;
				bool isEnd = false;
		};
};

class Substring: public String {
	public:
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

		StringIterator begin() const final {
			return StringIterator{std::make_unique<Iterator>(*this, false)};
		}

		StringIterator end() const final {
			return StringIterator{std::make_unique<Iterator>(*this, true)};
		}

	private:
		class Iterator: public IteratorImpl {
			public:
				Iterator(const Substring &owner, bool fakeEnd):
					owner(owner),
					subimpl(owner.parent->begin().impl),
					index(fakeEnd? -1 : 0) {}

				Type getType() const final { return Type::Substring; }

				void next() final {
					assert(!atEnd());
					accumulated += (**subimpl).size();
					++index;
					subimpl->next();
				}

				std::unique_ptr<IteratorImpl> clone() final {
					return std::unique_ptr<Iterator>{new Iterator{owner, subimpl->clone(), index, accumulated}};
				}

				std::string_view operator*() final {
					std::string_view view = **subimpl;
					while (view.size() + accumulated <= owner.offset) {
						next();
						if (subimpl->atEnd()) {
							return {};
						}
						view = **subimpl;
					}
					return view.substr(owner.offset - accumulated, owner.extent);
				}

				bool atEnd() const final {
					return index == -1 || subimpl->atEnd();
				}

				bool operator==(const IteratorImpl &other) const final {
					if (this == &other) {
						return true;
					}

					if (other.getType() != Type::Substring) {
						return false;
					}

					auto &substring = static_cast<const Iterator &>(other);
					return &owner == &substring.owner && ((atEnd() && other.atEnd()) || (index == substring.index && *subimpl == *substring.subimpl));
				}

			private:
				const Substring &owner;
				std::unique_ptr<IteratorImpl> subimpl;
				size_t accumulated = 0;
				int index = 0;

				Iterator(const Substring &owner, std::unique_ptr<IteratorImpl> subimpl, int index, size_t accumulated):
					owner(owner),
					subimpl(std::move(subimpl)),
					accumulated(accumulated),
					index(index) {}
		};
};

class Rope: public String {
	public:
		std::array<String *, 3> fibers{};
		int fiberCount = 0;

		Rope(String &fiber0):
			String(fiber0.size),
			fibers{&fiber0, nullptr, nullptr},
			fiberCount(1) {}

		Rope(String &fiber0, String &fiber1):
			String(fiber0.size + fiber1.size),
			fibers{&fiber0, &fiber1, nullptr},
			fiberCount(2) {}

			Rope(String &fiber0, String &fiber1, String &fiber2):
			String(fiber0.size + fiber1.size + fiber2.size),
			fibers{&fiber0, &fiber1, &fiber2},
			fiberCount(3) {}

		StringIterator begin() const final {
			return StringIterator{std::make_unique<Iterator>(*this, false)};
		}

		StringIterator end() const final {
			return StringIterator{std::make_unique<Iterator>(*this, true)};
		}

	private:
		class Iterator: public IteratorImpl {
			public:
				Iterator(const Rope &owner, bool fakeEnd):
					owner(owner),
					subimpl(owner.fibers[0]->begin().impl),
					fiberIndex(fakeEnd? 3 : 0) {}

				Type getType() const final { return Type::Rope; }

				void next() final {
					assert(!atEnd());

					while (subimpl->atEnd()) {
						assert(fiberIndex < owner.fiberCount - 1);
						subimpl = owner.fibers[++fiberIndex]->begin().impl;
					}

					accumulated += (**subimpl).size();
					++index;
					subimpl->next();

					if (subimpl->atEnd() && !atEnd()) {
						assert(fiberIndex < owner.fiberCount - 1);
						subimpl = owner.fibers[++fiberIndex]->begin().impl;
					}
				}

				std::unique_ptr<IteratorImpl> clone() final {
					return std::unique_ptr<IteratorImpl>{new Iterator{owner, subimpl->clone(), index, fiberIndex, accumulated}};
				}

				std::string_view operator*() final {
					return **subimpl;
				}

				bool atEnd() const final {
					return fiberIndex >= 3 || (fiberIndex == owner.fiberCount - 1 && subimpl->atEnd());
				}

				bool operator==(const IteratorImpl &other) const final {
					if (this == &other) {
						return true;
					}

					if (other.getType() != Type::Rope) {
						return false;
					}

					auto &rope = static_cast<const Iterator &>(other);
					return &owner == &rope.owner && ((atEnd() && other.atEnd()) || (index == rope.index && fiberIndex == rope.fiberIndex && *subimpl == *rope.subimpl));
				}

			private:
				const Rope &owner;
				std::unique_ptr<IteratorImpl> subimpl;
				size_t accumulated = 0;
				int index = 0;
				int fiberIndex = 0;

				Iterator(const Rope &owner, std::unique_ptr<IteratorImpl> subimpl, int index, int fiberIndex, size_t accumulated):
					owner(owner),
					subimpl(std::move(subimpl)),
					accumulated(accumulated),
					index(index),
					fiberIndex(fiberIndex) {}
		};
};

bool String::operator==(const String &other) const {
	if (this == &other) {
		return true;
	}

	if (size != other.size) {
		return false;
	}

	auto iter1 = begin();
	auto end1 = end();
	auto iter2 = other.begin();
	auto end2 = other.end();

	if (iter1 == end1 || iter2 == end2) {
		assert(iter1 == end1);
		assert(iter2 == end2);
		return false;
	}

	std::string_view view1 = *iter1;
	std::string_view view2 = *iter2;

	auto advance = [](auto &iter, const auto &end, auto &view) {
		do {
			if (++iter == end) {
				break;
			}

			view = *iter;
		} while (view.empty());
	};

	for (;;) {
		if (iter1 == end1 && iter2 == end2) {
			return true;
		}

		if (view1.size() < view2.size()) {
			if (view1 != view2.substr(0, view1.size())) {
				return false;
			}

			view2.remove_prefix(view1.size());
			advance(iter1, end1, view1);
		} else if (view1.size() > view2.size()) {
			if (view1.substr(0, view2.size()) != view2) {
				return false;
			}

			view1.remove_prefix(view2.size());
			advance(iter2, end2, view2);
		} else if (view1 == view2) {
			advance(iter1, end1, view1);
			advance(iter2, end2, view2);
		} else {
			return false;
		}
	}
}

std::string String::resolve(size_t pos, size_t n) {
	std::string out;

	size_t to_skip = pos;

	for (std::string_view view: *this) {
		if (view.size() <= to_skip) {
			to_skip -= view.size();
			continue;
		}

		if (to_skip > 0) {
			view.remove_prefix(to_skip);
			to_skip = 0;
		}

		if (out.size() + view.size() > n) {
			out += view.substr(0, n - out.size());
			break;
		}

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
