/* This program was created in response to a question about whether there could be a better way to handle errors than
 * the plain C way if you have no exceptions and no standard library. (The standard library includes here are for the
 * final example, which relies on an RAII-ish type to demonstrate the necessity of ref-qualifiers to specify overloads
 * for rvalues.
 *
 * Output:
 *     (&)
 *     m1 data: 42
 *     (&)
 *     (&)
 *     (&)
 *     m2 data data: 42
 *     m3 error: Nope.
 *     m4 error: Not here either.
 *     (&)
 *     m5 data: 69
 *     (&&)
 *     666
 *     (&&)
 *     some string ineligible for short string optimization
 */

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>

#ifndef _NEW
inline void * operator new(size_t, void *p) noexcept {
	return p;
}
#endif

/** Err is presumed to be contextually convertible to bool. */
template <typename T, typename Err = const char *>
class Maybe {
	private:
		// Nonsense to allow us to use non-default-constructible types for T.
		struct alignas(T) Data {
			char bytes[sizeof(T)];
		};

		Data data_;
		Err error_{};
		bool success_{};

		Maybe() = default;

	public:
		Maybe(T data): success_(true) {
			new (data_.bytes) T(static_cast<T &&>(data));
		}

		Maybe(Err err):
			error_(static_cast<Err &&>(err)) {}

		explicit inline operator bool() const {
			return success_;
		}

		const Err & error() const {
			return error_;
		}

		T & data() & {
			printf("\x1b[2m(&)\x1b[22m\n");
			if (!success_)
				abort();
			return *reinterpret_cast<T *>(data_.bytes);
		}

		T data() && {
			printf("\x1b[2m(&&)\x1b[22m\n");
			if (!success_)
				abort();
			return static_cast<T &&>(*reinterpret_cast<T *>(data_.bytes));
		}

		const T & data() const & {
			if (!success_)
				abort();
			return *reinterpret_cast<const T *>(data_.bytes);
		}
};

struct NoDefault {
	int data;
	NoDefault() = delete;
	NoDefault(int data_):
		data(data_) {}
};

int main() {
	Maybe m1{42};
	Maybe<Maybe<int>> m2{m1};
	Maybe<float> m3{"Nope."};
	Maybe<NoDefault> m4{"Not here either."};
	Maybe<NoDefault> m5{69};

	if (m1) {
		printf("m1 data: %d\n", m1.data());
	} else {
		printf("m1 error: %s\n", m1.error());
	}

	if (auto error = m2.error()) {
		printf("m2 error: %s\n", error);
	} else if (m2.data()) {
		printf("m2 data data: %d\n", m2.data().data());
	} else {
		printf("m2 data error: %s\n", m2.data().error());
	}

	if (auto error = m3.error()) {
		printf("m3 error: %s\n", error);
	} else {
		printf("m3 data: %f\n", m3.data());
	}

	if (auto error = m4.error()) {
		printf("m4 error: %s\n", error);
	} else {
		printf("m4 data: %d\n", m4.data().data);
	}

	if (auto error = m5.error()) {
		printf("m5 error: %s\n", error);
	} else {
		printf("m5 data: %d\n", m5.data().data);
	}

	printf("%d\n", Maybe<int>(666).data());
	std::cout << Maybe(std::string("some string ineligible for short string optimization")).data() << '\n';

	// Ill-formed.
	// Maybe<void> mv("Surely it's okay to use Maybe<void> if there's only an error!");
	// if (auto error = mv.error()) {
	//     printf("mv error: %s\n", error);
	// } else {
	//     printf("mv: no error.\n");
	// }
}
