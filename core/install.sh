#!/bin/bash

shopt -s globstar

SRC=$(ls src/**/*.cpp)

mkdir -p build

for f in $SRC; do 
	base=$(basename -s .cpp $f)
	g++ -g -o "build/${base}.o" -c -Iinclude $f || exit 3
done

ar rcs build/libforg.a build/*.o || exit 3
# g++ -shared -o libforg.so build/*.o || exit 3

# for release builds
(sudo install build/libforg.a /usr/local/lib/) || exit 3
# for development(?)
# (sudo install build/libforg.so /usr/local/lib/) || exit 3
(sudo install -d /usr/local/include/forg/) || exit 3
(sudo install include/*.hpp /usr/local/include/forg/) || exit 3
