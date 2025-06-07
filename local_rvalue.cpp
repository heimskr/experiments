#include <memory>
#include <print>
#include <string>

#include <cxxabi.h>

std::string demangle(const char *name) {
	int status = INT_MIN;
	std::unique_ptr<char[], void(*)(void *)> res(abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
	std::string out = status == 0? res.get() : name;
	if (out == "Wrapper<char const*>") {
		out = "Str";
	} else if (out == "Wrapper<Wrapper<char const*> >" || out == "Wrapper<Str>") {
		out = "StrStr";
	} else if (out == "char const*") {
		out = "const char *";
	}
	return out;
}

template <typename T>
struct Wrapper: T {
	using Sub = T;

	std::string name = demangle(typeid(Wrapper<T>).name());
	std::string subname = demangle(typeid(T).name());

	Wrapper() {
		std::println("\t\x1b[2m[ {}() ]\x1b[22m {}", name, (void *) this);
	}

	~Wrapper() {
		std::println("\t\x1b[2m[ ~{}() ]\x1b[22m {}", name, (void *) this);
	}

	Wrapper(const T &other):
		T(other) {
			std::println("\t\x1b[2m[ {}(const {} &) ]\x1b[22m {}", name, subname, (void *) this);
		}

	Wrapper(T &&other):
		T(std::move(other)) {
			std::println("\t\x1b[2m[ {}({} &&) ]\x1b[22m {}", name, subname, (void *) this);
		}

	Wrapper(const Wrapper<T> &other):
		T(other) {
			std::println("\t\x1b[2m[ {}(const {} &) ]\x1b[22m {}", name, name, (void *) this);
		}

	Wrapper(Wrapper<T> &&other):
		T(std::move(other)) {
			std::println("\t\x1b[2m[ {}({} &&) ]\x1b[22m {}", name, name, (void *) this);
		}

	Wrapper<T> & operator=(const T &other) {
		std::println("\t\x1b[2m[ {} & operator=(const {} &) ]\x1b[22m {}", name, subname, (void *) this);
		T::operator=(other);
		return *this;
	}

	Wrapper<T> & operator=(T &&other) {
		std::println("\t\x1b[2m[ {} & operator=({} &&) ]\x1b[22m {}", name, subname, (void *) this);
		T::operator=(std::move(other));
		return *this;
	}

	Wrapper<T> & operator=(const Wrapper<T> &other) {
		std::println("\t\x1b[2m[ {} & operator=(const {} &) ]\x1b[22m {}", name, name, (void *) this);
		T::operator=(other);
		return *this;
	}

	Wrapper<T> & operator=(Wrapper<T> &&other) {
		std::println("\t\x1b[2m[ {} & operator=({} &&) ]\x1b[22m {}", name, name, (void *) this);
		T::operator=(std::move(other));
		return *this;
	}
};

template <>
struct Wrapper<const char *> {
	using Sub = const char *;

	std::string value;
	std::string name = demangle(typeid(*this).name());
	std::string subname = demangle(typeid(Sub).name());

	Wrapper() {
		std::println("\t\x1b[2m[ {}() ]\x1b[22m {}", name, (void *) this);
	}

	~Wrapper() {
		std::println("\t\x1b[2m[ ~{}() ]\x1b[22m {}", name, (void *) this);
	}

	Wrapper(const char * const &other):
		value(other) {
			std::println("\t\x1b[2m[ {}(const {} &) ]\x1b[22m {}", name, subname, (void *) this);
		}

	Wrapper(const char * &&other):
		value(std::move(other)) {
			std::println("\t\x1b[2m[ {}({} &&) ]\x1b[22m {}", name, subname, (void *) this);
		}

	Wrapper(const Wrapper<const char *> &other):
		value(other.value) {
			std::println("\t\x1b[2m[ {}(const {} &) ]\x1b[22m {}", name, name, (void *) this);
		}

	Wrapper(Wrapper<const char *> &&other):
		value(std::move(other.value)) {
			std::println("\t\x1b[2m[ {}({} &&) ]\x1b[22m {}", name, name, (void *) this);
		}

	Wrapper<const char *> & operator=(const char * const &other) {
		std::println("\t\x1b[2m[ {} & operator=(const {} &) ]\x1b[22m {}", name, subname, (void *) this);
		value = other;
		return *this;
	}

	Wrapper<const char *> & operator=(const char * &&other) {
		std::println("\t\x1b[2m[ {} & operator=({} &&) ]\x1b[22m {}", name, subname, (void *) this);
		value = std::move(other);
		return *this;
	}

	Wrapper<const char *> & operator=(const Wrapper<const char *> &other) {
		std::println("\t\x1b[2m[ {} & operator=(const {} &) ]\x1b[22m {}", name, name, (void *) this);
		value = other.value;
		return *this;
	}

	Wrapper<const char *> & operator=(Wrapper<const char *> &&other) {
		std::println("\t\x1b[2m[ {} & operator=({} &&) ]\x1b[22m {}", name, name, (void *) this);
		value = std::move(other.value);
		return *this;
	}
};

using Str = Wrapper<const char *>;

using StrStr = Wrapper<Str>;

template <>
struct std::formatter<Str> {
	constexpr auto parse(auto &ctx) {
		return ctx.begin();
	}

	auto format(const Str &str, auto &ctx) const {
		return std::format_to(ctx.out(), "{}", str.value);
	}
};

template <>
struct std::formatter<StrStr> {
	constexpr auto parse(auto &ctx) {
		return ctx.begin();
	}

	auto format(const StrStr &str, auto &ctx) const {
		return std::format_to(ctx.out(), "<{}>", str.value);
	}
};


struct Test {
	Test() {
		std::println("\x1b[1m<<<<<<<<<<<<<<<<\x1b[22m\n");
	}

	~Test() {
		std::println("\n\x1b[1m>>>>>>>>>>>>>>>>\x1b[22m");
	}
};

template <typename... Args>
void print(Args &&...args) {
	(std::println("  {}", args), ...);
}

int main() {
	{
		Test test;
		std::println("StrStr &&a = Str(\"foo\");"); StrStr &&a = Str("foo");
		std::println("print(a);");                          print(a);
	}

	// learnings:
	// rvalue reference variables are kind of like lvalue references except they can bind only to xvalues. they don't cause any constructions or assignments on their own.
}
