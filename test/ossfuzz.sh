#!/bin/bash -eu
# This script is meant to be run by
# https://github.com/google/oss-fuzz/blob/master/projects/utf8proc/Dockerfile

mkdir build
cd build
cmake ..
make -j$(nproc)

$CC $CFLAGS -I$SRC/utf8proc \
    $SRC/utf8proc/test/fuzzer.c \
    -o $OUT/utf8proc_fuzzer \
    $LIB_FUZZING_ENGINE $SRC/utf8proc/build/libutf8proc.a

find $SRC/utf8proc/test -name "*.txt" | \
     xargs zip $OUT/utf8proc_fuzzer_seed_corpus.zip
