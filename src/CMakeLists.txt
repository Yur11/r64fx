cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -DR64FX_DEBUG -DR64FX_USE_X11")

set(SRC
    Image.cpp
    Keyboard.cpp
    Program.cpp
    Window.cpp
    WindowX11.cpp
    Widget.cpp
    main.cpp
)

add_executable(r64x ${SRC})

set(LIBS
    X11
    GL
)

target_link_libraries(r64x ${LIBS})
