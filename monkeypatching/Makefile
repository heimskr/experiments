CXXFLAGS ?= -std=c++23 -Wall -Wextra

%.so: %.cpp base.h
	clang++ $(CXXFLAGS) -fpic -c $(@:.so=.cpp) -o $(@:.so=.o)
	clang++ -shared $(@:.so=.o) -o $@

main: main.cpp base.so override.so
	clang++ $(CXXFLAGS) main.cpp -o main base.so

clean:
	rm -f main base.so base.o override.so override.o

test: main
	./main