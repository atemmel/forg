#!/bin/sh

headers=$(ls include/*.hpp)

printf "#pragma once\n\n" > include/core.hpp

for header in $headers; do
	base=$(basename $header)
	echo "#include \"${base}\"" >> include/core.hpp
done
