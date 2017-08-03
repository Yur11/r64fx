#!/bin/bash

#Run from src/ directory.

if [ ! -x ./makestring ]
then
    g++ -std=c++11 -O0 ../tools/makestring.cpp -o makestring
fi

MAKESTRINGCMD="`pwd`/makestring"
echo $MAKESTRINGCMD

if [ "$#" != "0" ]; then cd $1; fi
for i in `find . -name "*.vert" -o -name "*.frag"`
do
    `echo $MAKESTRINGCMD` < $i > $i.h
    echo "$i.h"
done
if [ "$#" != "0" ]; then cd -; fi
