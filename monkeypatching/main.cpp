#include "base.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <print>
#include <sys/mman.h>
#include <typeinfo>
#include <unistd.h>

void monkey_patch(void *sym, void *jump_target, int offset = 0) {
	// https://blog.llandsmeer.com/tech/2019/06/25/overwrite-a-function.html

    static int PAGE_SIZE = 0;

    if (PAGE_SIZE == 0) {
		PAGE_SIZE = getpagesize();
	}

    void *page = (void *) ((uintptr_t) sym & (uintptr_t) ~(PAGE_SIZE - 1));

    struct {
        unsigned char jmp_qword_ptr_rip[6];
        uint64_t addr;
    } __attribute__((packed)) asm_jmp_abs = {
        {0xff, 0x25, 0, 0, 0, 0}, (uint64_t) jump_target
    };

    mprotect(page, 2 * PAGE_SIZE, PROT_WRITE);
    void *target = (void *) ((uintptr_t) sym + offset);
    memcpy(target, &asm_jmp_abs, sizeof asm_jmp_abs);
    mprotect(page, 2 * PAGE_SIZE, PROT_READ | PROT_EXEC);
}

void monkey_patch_addr(const char *function, void *jump_target, int offset = 0) {
    static void *handle = dlopen(nullptr, RTLD_LAZY);
    void *sym = dlsym(handle, function);
    monkey_patch(sym, jump_target, offset);
}

void monkey_patch(const std::string &function, void *handle) {
	monkey_patch_addr(function.c_str(), dlsym(handle, function.c_str()));
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
	void *so = dlopen("override.so", RTLD_NOW);
	assert(so != nullptr);
	std::string name = mangle(&Foo::bar, "bar");
	std::println("name = {}", name);
	std::println("horror = {}", mangle(&Foo::baz<const int, volatile char>, "baz"));
	monkey_patch(mangle_fn(&hej), so);
	monkey_patch(name, so);
	hej();
	std::string after = "after";
	const std::string *after_p = &after;
	foo.bar(64, &after_p, 10, nullptr);
}
