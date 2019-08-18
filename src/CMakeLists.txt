cmake_minimum_required(VERSION 3.3)
project(r64fx)

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "-std=c++17")

### Pick one of these!
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -g -fno-omit-frame-pointer")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -g -fno-omit-frame-pointer -fsanitize=address")

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
    Atlas.cpp
    Clipboard.cpp
    ClipboardEvents.cpp
    Conf_Scale.cpp
    Directory.cpp
#     DummyMachine.cpp
#     FilterMachine.cpp
    FilterClass.cpp
    Font.cpp
    FontSupply.cpp
    FrequencyPlot.cpp
    IconSupply.cpp
    Image.cpp
    ImageAnimation.cpp
    ImageUtils.cpp
    InstanceCounter.cpp
    Keyboard.cpp
    KeyboardModifiers.cpp
    LayoutUtils.cpp
    Main.cpp
    MainWindow.cpp
    MemoryUtils.cpp
    Midi.cpp
    MidiDispatcher.cpp
    Module.cpp
    Mouse.cpp
    Mutex.cpp
    Painter.cpp
    PainterShaders.cpp
    PainterVertexArrays.cpp
#     Program.cpp
    RingSectorPainter.cpp
    Shader.cpp
    SoundDriver.cpp
    SoundFile.cpp
    SoundFileLoader.cpp
    StringUtils.cpp
    Strings_Eng.cpp
    SystemUtils.cpp
    TextEditingUtils.cpp
    TextPainter.cpp
    Thread.cpp
    TimeUtils.cpp
    Timer.cpp
    UndoRedoChain.cpp
    UndoRedoUtils.cpp
    Value.cpp
#     View_Filter.cpp
#     View_Player.cpp
    WaveformUtils.cpp
    Widget.cpp
    Widget_Button.cpp
    Widget_CheckBox.cpp
    Widget_ItemBrowser.cpp
    Widget_DataItem.cpp
    Widget_DirectoryItem.cpp
    Widget_Dummy.cpp
    Widget_Knob.cpp
    Widget_Label.cpp
    Widget_Menu.cpp
    Widget_Number.cpp
    Widget_ScrollArea.cpp
    Widget_ScrollBar.cpp
    Widget_Slider.cpp
    Widget_TabBar.cpp
    Widget_Text.cpp
    Window.cpp
    gl.cpp
    jit.cpp
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


#########################################################################################
## Some Tests ###########################################################################
#########################################################################################
add_custom_target(tests DEPENDS
    test_LinkedList test_Timer test_MemoryUtils test_jit test_SignalGraph test_SoundDriver
)

## test_LinkedList ##
set(test_LinkedList_SRC
    test_LinkedList.cpp
)

add_executable(test_LinkedList EXCLUDE_FROM_ALL ${test_LinkedList_SRC})


## test_Timer ##
set(test_Timer_SRC
    test_Timer.cpp
    Timer.cpp
    Thread.cpp
    Mutex.cpp
    TimeUtils.cpp
)

add_executable(test_Timer EXCLUDE_FROM_ALL ${test_Timer_SRC})

set(test_Timer_LIBS pthread)

target_link_libraries(test_Timer ${test_Timer_LIBS})


## test_MemoryUtils ##
set(test_MemoryUtils_SRC
    test_MemoryUtils.cpp
    MemoryUtils.cpp
)

add_executable(test_MemoryUtils EXCLUDE_FROM_ALL ${test_MemoryUtils_SRC})

## test_MidiEventBuffer ##
set(test_MidiEventBuffer_SRC
    test_MidiEventBuffer
    Midi.cpp
)

add_executable(test_MidiEventBuffer EXCLUDE_FROM_ALL ${test_MidiEventBuffer_SRC})


## test_jit ##
set(test_jit_SRC
    test_jit.cpp
    jit.cpp
    MemoryUtils.cpp
)

add_executable(test_jit EXCLUDE_FROM_ALL ${test_jit_SRC})


## test_jit_perf ##
set(test_jit_perf_SRC
    test_jit_perf.cpp
    jit.cpp
    MemoryUtils.cpp
)

add_executable(test_jit_perf EXCLUDE_FROM_ALL ${test_jit_perf_SRC})


## test_SoundDriver ##
set(test_SoundDriver_SRC
    test_SoundDriver.cpp
    SoundDriver.cpp
    Midi.cpp
    MemoryUtils.cpp
    TimeUtils.cpp
    Thread.cpp
)

add_executable(test_SoundDriver EXCLUDE_FROM_ALL ${test_SoundDriver_SRC})

set(test_SoundDriver_LIBS jack pthread)

target_link_libraries(test_SoundDriver ${test_SoundDriver_LIBS})


## test_Synth ##
set(test_Synth_SRC
    test_Synth.cpp
    SoundDriver.cpp
    Midi.cpp
    MemoryUtils.cpp
    TimeUtils.cpp
    Thread.cpp
    jit.cpp
)

add_executable(test_Synth EXCLUDE_FROM_ALL ${test_Synth_SRC})

set(test_Synth_LIBS jack pthread)

target_link_libraries(test_Synth ${test_Synth_LIBS})



## test_jit_procedures ##
set(test_jit_procedures_SRC
    test_jit_procedures.cpp
    jit_procedures.cpp
    Midi.cpp
    MemoryUtils.cpp
    TimeUtils.cpp
    Thread.cpp
    jit.cpp
)

add_executable(test_jit_procedures EXCLUDE_FROM_ALL ${test_jit_procedures_SRC})

set(test_jit_procedures_LIBS pthread)

target_link_libraries(test_jit_procedures ${test_jit_procedures_LIBS})
