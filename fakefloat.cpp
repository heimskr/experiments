// This program demonstrates printf's lack of type safety.

#include <stdio.h>

int main() {
	printf("%f\n", 0x400921fb4d12d84aul);
	printf("%x\n", 3.1415926f);
}
