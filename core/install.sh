#!/bin/sh

shopt -s globstar

SRC=$(ls src/**/*.cpp)

mkdir -p build

for f in $SRC; do 
	base=$(basename -s .cpp $f)
	g++ -g -o "build/${base}.o" -c -Iinclude $f || exit 3
done

ar rcs build/libforg.a build/*.o || exit 3

(sudo install build/libforg.a /usr/lib/) || exit 3
(sudo install -d /usr/include/forg/) || exit 3
(sudo install include/*.hpp /usr/include/forg/) || exit 3
