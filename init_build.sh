#!/bin/bash
SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo $SRC_DIR > src_dir
# ln -s $SRC_DIR/tools/Makefile
g++ -std=c++11 -O2 \
    $SRC_DIR/tools/build_tool.cpp \
    $SRC_DIR/util/Arg.cpp \
    $SRC_DIR/util/Dir.cpp \
    -I $SRC_DIR \
    -o build_tool