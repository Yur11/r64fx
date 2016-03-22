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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_JACK")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_PTHREAD")

set(SRC
    Clipboard.cpp
    ClipboardEvent.cpp
    Font.cpp
    Image.cpp
    ImageAnimation.cpp
    ImagePainter.cpp
    ImageUtils.cpp
    Keyboard.cpp
    KeyboardModifiers.cpp
    Midi.cpp
    Mouse.cpp
    Mutex.cpp
    Painter.cpp
    Program.cpp
    SignalData.cpp
    Shader.cpp
    Shader_rgba.cpp
    Shader_rgba_tex.cpp
    SoundDriver.cpp
    StringUtils.cpp
    TextPainter.cpp
    Thread.cpp
    Timer.cpp
    UndoRedoChain.cpp
    Widget.cpp
    Widget_Button.cpp
    Widget_Container.cpp
    Widget_Control.cpp
    Widget_Dummy.cpp
    Widget_ScrollBar.cpp
    Widget_Text.cpp
    Widget_View.cpp
    Window.cpp
    current_time.cpp
    gl.cpp
    main.cpp
    sleep.cpp
)

add_executable(r64x ${SRC})

set(LIBS
    X11
    Xext
    GL
    freetype
    sndfile
    jack
    pthread
)

target_link_libraries(r64x ${LIBS})
