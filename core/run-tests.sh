#!/bin/sh

shopt -s globstar

SRC=$(ls src/**/*.cpp)
TEST_SRC=$(ls tests/**/*.cpp)
TEST_FLAGS="-g -O0 --coverage"

g++ -o test $TEST_FLAGS -Iinclude -lCatch2Main $SRC $TEST_SRC -lCatch2

# gcovr
