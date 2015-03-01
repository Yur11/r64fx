#!/bin/sh

##
# Configure variables.
##

PROJECT_NAME="r64fx"

DEFAULT_PROJECT_COMPILER="g++ -std=c++11"

DEFAULT_DEFS="-DEBUG -DUSE_SDL2 -DDEBUG_GL_ERRORS -DABORT_ON_GL_ERROR"

DEFAULT_LINK_FLAGS="-lGL -lGLU -lGLEW -lSDL2 -lpng -lfreetype -lsndfile -ljack -lX11"

BUILD_DIR=`pwd`

SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ "$BUILD_DIR" == "$SRC_DIR" ]
then
    printf "Refusing to do an in-source build!\nPlease use another directory!\n\nNOTE:  You can create and use a subdirectory.\n\n" 1>&2
    exit 1
fi

echo "$SRC_DIR" > SRC_DIR
echo "$BUILD_DIR" > BUILD_DIR

if [ ! -f PROJECT_COMPILER ]
then
    PROJECT_COMPILER="g++ -std=c++11"
    echo "Project compiler is \"$PROJECT_COMPILER\""
    echo "$PROJECT_COMPILER" > PROJECT_COMPILER
else
    PROJECT_COMPILER=`cat PROJECT_COMPILER`
fi

if [ ! -f INCLUDE_FLAGS ]
then
    INCLUDE_FLAGS="-I${BUILD_DIR} -I$SRC_DIR"
    echo "$INCLUDE_FLAGS" > INCLUDE_FLAGS
fi

if [ ! -f DEFS ]
then
    DEFS="$DEFAULT_DEFS"
    echo "$DEFS" > DEFS
fi

if [ ! -f LINK_FLAGS ]
then
    LINK_FLAGS="$DEFAULT_LINK_FLAGS"
    echo "$LINK_FLAGS" > LINK_FLAGS
fi


##
# Build tools if needed!!
##

function build_tool()
{
    echo "Building    $1"
    $PROJECT_COMPILER -O2 $SRC_DIR/tools/$1.cpp -o $1
}

function ensure_tool_exists()
{
    if [ ! -f $1 ]
    then
        build_tool $1
    fi
}

if  [ "$1" == 'maketool' ] || [ "$1" == 'conftool' ]
then
    build_tool $1
    exit 0
else
    for i in "maketool" "conftool"
    do
        ensure_tool_exists $i
    done
fi


##
# Process main project file!!
##

if [ ! -f "$SRC_DIR/$PROJECT_NAME.project" ]
then
    echo "Failed to find $PROJECT_NAME.project file!\n"
    exit 1
fi

./maketool $@ < $SRC_DIR/$PROJECT_NAME.project