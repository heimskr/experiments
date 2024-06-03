#!/bin/bash

clang++ -std=c++20 -Wall -Wextra -g $1 $2 -o /tmp/$(basename "$2").x && /tmp/$(basename "$2").x ${@:3}
rm -f /tmp/$(basename "$2").x
