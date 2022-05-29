#!/bin/bash

root=$PWD

files="
$root/code/main.cpp
"

includes="
-I/usr/local/include/
"

libs="
-L/usr/local/lib
-lm
-lX11
-lGL
"

binary=$root/build/sandbox
flags="-O0 -g3"

mkdir -p $root/build/
clang $files $includes $flags $libs -o $binary
