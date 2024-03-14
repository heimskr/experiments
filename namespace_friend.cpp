#include <iostream>
#include <string>

namespace NA {
	class A {
		private:
			int x = 0;

		friend class B;
	};

	struct B {
		static int access(const A &a) {
			return a.x;
		}
	};
}

namespace NB {
	struct B {
		static int access(const NA::A &a) {
			return a.x;
		}
	};
}

int main() {}
