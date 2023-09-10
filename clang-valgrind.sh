#!/bin/bash

clang++ -std=c++20 -Wall -Wextra -g -O0 $1 -o /tmp/$(basename "$1").x && valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all /tmp/$(basename "$1").x ${@:2}
rm -f /tmp/$(basename "$1").x
