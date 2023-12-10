#!/bin/sh

shopt -s globstar

SRC=$(ls src/**/*.cpp)
TEST_SRC=$(ls tests/**/*.cpp)

g++ -o test -g -Iinclude -lCatch2Main $SRC $TEST_SRC -lCatch2
