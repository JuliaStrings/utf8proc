#!/bin/bash -eu
# This script is meant to be run by
# https://github.com/google/oss-fuzz/blob/master/projects/utf8proc/Dockerfile

mkdir build
cd build
cmake .. -DUTF8PROC_ENABLE_TESTING=ON
make -j$(nproc)

cp $SRC/utf8proc/build/fuzzer utf8proc_fuzzer

find $SRC/utf8proc/test -name "*.txt" | \
     xargs zip $OUT/utf8proc_fuzzer_seed_corpus.zip