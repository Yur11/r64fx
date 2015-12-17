cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_DEBUG")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_X11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_MITSHM")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_GL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_DEBUG_GL_ERRORS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_ABORT_ON_GL_ERROR")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/freetype2/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_BUILTIN_FONTS")

set(SRC
    AudioData.cpp
    Font.cpp
    Image.cpp
    ImageUtils.cpp
    Keyboard.cpp
    Mouse.cpp
    Shader.cpp
    Shader_rgba.cpp
    Shader_rgba_tex.cpp
    StringUtils.cpp
    Painter.cpp
    Program.cpp
    Widget.cpp
    Widget_Dummy.cpp
    Widget_Text.cpp
    Window.cpp
    gl.cpp
    main.cpp
)

add_executable(r64x ${SRC})

set(LIBS
    X11
    Xext
    GL
    freetype
    sndfile
)

target_link_libraries(r64x ${LIBS})
