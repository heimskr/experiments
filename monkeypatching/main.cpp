#include "base.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <print>
#include <sys/mman.h>
#include <typeinfo>
#include <unistd.h>

static const int PAGE_SIZE = getpagesize();

struct AsmJmp {
	unsigned char jmp_qword_ptr_rip[6];
	uint64_t addr;
} __attribute__((packed));

using AsmBytes = std::array<std::byte, sizeof(AsmJmp)>;

class Patch {
	public:
		Patch(void *page, void *target, const AsmBytes &old):
			page(page),
			target(target),
			old(old) {}

		void swap() {
			AsmBytes new_old;
			mprotect(page, 2 * PAGE_SIZE, PROT_WRITE);
			std::memcpy(new_old.data(), target, sizeof(AsmJmp));
			std::memcpy(target, old.data(), sizeof(AsmJmp));
			mprotect(page, 2 * PAGE_SIZE, PROT_READ | PROT_EXEC);
			old = new_old;
		}

	private:
		void *page = nullptr;
		void *target = nullptr;
		AsmBytes old{};
};

Patch monkey_patch(void *sym, void *jump_target, int offset = 0) {
	// https://blog.llandsmeer.com/tech/2019/06/25/overwrite-a-function.html
    void *page = (void *) ((uintptr_t) sym & (uintptr_t) ~(PAGE_SIZE - 1));
    AsmJmp asm_jmp_abs{{0xff, 0x25, 0, 0, 0, 0}, (uint64_t) jump_target};
    mprotect(page, 2 * PAGE_SIZE, PROT_WRITE);
    void *target = (void *) ((uintptr_t) sym + offset);
	AsmBytes old;
	std::memcpy(old.data(), target, sizeof(AsmJmp));
    std::memcpy(target, &asm_jmp_abs, sizeof(AsmJmp));
    mprotect(page, 2 * PAGE_SIZE, PROT_READ | PROT_EXEC);
	return {page, target, old};
}

decltype(auto) monkey_patch_addr(const char *function, void *jump_target, int offset = 0) {
    static void *handle = dlopen(nullptr, RTLD_LAZY);
    void *sym = dlsym(handle, function);
    return monkey_patch(sym, jump_target, offset);
}

decltype(auto) monkey_patch(const std::string &function, void *handle) {
	return monkey_patch_addr(function.c_str(), dlsym(handle, function.c_str()));
}

template <typename T>
std::string mangle_fn(const T &symbol) {
	Dl_info info{};
	auto addr = reinterpret_cast<const void *>(symbol);
	if (!dladdr(addr, &info)) {
		throw std::runtime_error(std::format("Couldn't mangle {}", addr));
	}
	return info.dli_sname;
}

template <typename T>
struct mangler {
	static std::string mangle() {
		using D = std::remove_reference_t<T>;
		return std::string(std::is_lvalue_reference_v<T>? "R" : std::is_rvalue_reference_v<T>? "O" : "") + (std::is_volatile_v<D>? "V" : "") + (std::is_const_v<D>? "K" : "") + typeid(D).name();
	}
};

template <typename R, typename C, typename... A>
std::string mangle(R (C::*)(A...), std::string_view n) {
	std::string out = std::format("_ZN{}{}{}E", typeid(C).name(), n.size(), n);
	((out += mangler<A>::mangle()), ...);
	return out;
}

template <typename R, typename C, typename... A>
std::string mangle(R (C::*)(A...) const, std::string_view n) {
	std::string out = std::format("_ZNK{}{}{}E", typeid(C).name(), n.size(), n);
	((out += mangler<A>::mangle()), ...);
	return out;
}

template <typename R, typename C, typename... A>
std::string mangle(R (C::*)(A...) volatile, std::string_view n) {
	std::string out = std::format("_ZNV{}{}{}E", typeid(C).name(), n.size(), n);
	((out += mangler<A>::mangle()), ...);
	return out;
}

template <typename R, typename C, typename... A>
std::string mangle(R (C::*)(A...) const volatile, std::string_view n) {
	std::string out = std::format("_ZNVK{}{}{}E", typeid(C).name(), n.size(), n);
	((out += mangler<A>::mangle()), ...);
	return out;
}

int main() {
	Foo foo{42};
	hej();
	std::string before = "before";
	const std::string *before_p = &before;
	foo.bar(64, &before_p, 10, nullptr);
	std::println();
	void *so = dlopen("override.so", RTLD_NOW);
	assert(so != nullptr);
	std::string name = mangle(&Foo::bar, "bar");
	std::println("name = {}", name);
	std::println("horror = {}", mangle(&Foo::baz<const int, volatile char>, "baz"));
	std::println("virtual = {}", mangle(&Foo::quux, "quux"));
	Patch hej_patch = monkey_patch(mangle_fn(&hej), so);
	monkey_patch(name, so);
	std::println();
	hej();
	std::string after = "after";
	const std::string *after_p = &after;
	foo.bar(64, &after_p, 10, nullptr);
	std::println();
	for (int i = 0; i < 5; ++i) {
		hej_patch.swap();
		hej();
	}
}
