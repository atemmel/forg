#!/bin/sh

shopt -s globstar

SRC=$(ls src/**/*.cpp)
TEST_SRC=$(ls tests/**/*.cpp)

g++ -o test -g -O0 -Iinclude -lCatch2Main $SRC $TEST_SRC -lCatch2 --coverage

# gcovr
