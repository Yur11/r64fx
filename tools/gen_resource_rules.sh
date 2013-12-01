#!/bin/bash
#Generate make rules for generating resources 
#with a $1 command for file that match the $2 pattern,
#in the list of directories that is set by the remaining cmd. args.

echo 
for i in $(find "${@:3:$#}" -name $2)
do
    TARGET_NAME=`basename $i`
    
    eval "$1 $i > $TARGET_NAME.h"
    
    printf "$TARGET_NAME.h: $1 $i\n"
    printf "\t@echo \"`basename $1` $TARGET_NAME.h\"\n"
    printf "\t@$1 $i > $TARGET_NAME.h\n\n"
done