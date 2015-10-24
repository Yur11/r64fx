cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_X11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_MITSHM")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_GL")

set(SRC
    Image.cpp
    Keyboard.cpp
    Painter.cpp
    Program.cpp
    Window.cpp
    WindowX11.cpp
    Widget.cpp
    main.cpp
)

add_executable(r64x ${SRC})

set(LIBS
    X11
    Xext
    GL
)

target_link_libraries(r64x ${LIBS})
