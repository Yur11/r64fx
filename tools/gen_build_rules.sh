#!/bin/bash
#Scan the source directories and generate makerules
#for building cpp files as well as building resources.

if [ ! "$1" ]
then
    echo "gen_build_list.sh: CXX has not been set!"
    echo "                   Set it as the first cmd. argument!"
    exit 1
fi

CXX=$1

SRC_DIRS=$(for i in ${@:2:$#}; do printf "`cat src_dir`/$i "; done)

global_object_file_list=""

for src_dir in $SRC_DIRS
do
    object_file_prefix=`basename $src_dir`
    printf "#Rules for $object_file_prefix.\n\n"
    
    cpp_files=$(find $src_dir -name *.cpp)
   
    object_file_list=""
    for cpp_file in $cpp_files
    do
        target_name="$(printf "$object_file_prefix"; printf "_`basename $cpp_file | sed -e 's/.cpp/.o/g'`")"
        eval "$CXX -MM `realpath $cpp_file` -MT $target_name"
        printf "\t@echo \"Building \$@\"\n"
        printf "\t@$CXX $CXX_FLAGS -c `realpath $cpp_file` -o $target_name\n\n"
        
        object_file_list="$object_file_list $target_name"
    done
    
    printf "\n\n`basename $src_dir`_object_files =$object_file_list\n\n"
    
    global_object_file_list="$global_object_file_list $object_file_list"
done

printf "object_files = $global_object_file_list\n\n"
