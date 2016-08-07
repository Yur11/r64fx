cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_FLAGS "-std=c++11")

### Pick one of these!
# set(CMAKE_CXX_FLAGS "-Wall -O2")
set(CMAKE_CXX_FLAGS "-Wall -O0 -g")
# set(CMAKE_CXX_FLAGS "-Wall -O0 -g -fsanitize=address -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_DEBUG")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_X11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_MITSHM")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_GL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_DEBUG_GL_ERRORS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_ABORT_ON_GL_ERROR")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/freetype2/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_BUILTIN_FONTS")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_XCURSOR")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_JACK")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DR64FX_USE_PTHREAD")

set(SRC
    Action.cpp
    Clipboard.cpp
    ClipboardEvent.cpp
    Directory.cpp
    DummyMachine.cpp
    Font.cpp
    HostView.cpp
    Image.cpp
    ImageAnimation.cpp
    ImageUtils.cpp
    Keyboard.cpp
    KeyboardModifiers.cpp
    LayoutUtils.cpp
    Machine.cpp
    MachinePoolContext.cpp
    Midi.cpp
    MidiDispatcher.cpp
    Mouse.cpp
    Mutex.cpp
    Painter.cpp
    PlayerView.cpp
    SignalData.cpp
    SignalGraph.cpp
    SignalNode.cpp
    SignalPort.cpp
    Shader.cpp
    Shader_rgba.cpp
    Shader_rgba_tex.cpp
    SoundDriver.cpp
    SoundDriverMachine.cpp
    SoundFile.cpp
    StringUtils.cpp
    TextPainter.cpp
    Thread.cpp
    Timer.cpp
    UndoRedoChain.cpp
    Widget.cpp
    Widget_Button.cpp
    Widget_ItemBrowser.cpp
    Widget_DataItem.cpp
    Widget_DirectoryItem.cpp
    Widget_Dummy.cpp
    Widget_Knob.cpp
    Widget_Label.cpp
    Widget_Menu.cpp
    Widget_ScrollArea.cpp
    Widget_ScrollBar.cpp
    Widget_Slider.cpp
    Widget_Text.cpp
    Window.cpp
    current_time.cpp
    gl.cpp
    main.cpp
    sleep.cpp
)

add_executable(r64fx ${SRC})

set(LIBS
    X11
    Xext
    Xcursor
    GL
    freetype
    sndfile
    jack
    pthread
)

target_link_libraries(r64fx ${LIBS})
