#!/bin/bash

g++ -std=c++20 -Wall -Wextra -g -O0 $1 -o /tmp/$(basename "$1").x && /tmp/$(basename "$1").x ${@:2}
rm -f /tmp/$(basename "$1").x
