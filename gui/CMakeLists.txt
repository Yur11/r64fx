cmake_minimum_required(VERSION 2.8)
project(r64fx_gui)

add_library(r64fx_gui
    SHARED
    WindowBase.cpp
    Window.cpp
    View.cpp
    Scene.cpp
    Widget.cpp
    containers.cpp
    Dummy.cpp
    TextLine.cpp
    TextEdit.cpp
    Icon.cpp
    Texture.cpp
    Config.cpp
    MenuItem.cpp
    Menu.cpp
    Font.cpp
    Message.cpp
    Keyboard.cpp
    KeyboardShortcut.cpp
) 
# add_executable(r64fx_gui_example example.cpp)

target_link_libraries(r64fx_gui SDL2 GL GLU ftgl png Utf8String)

# target_link_libraries(r64fx_gui_example r64fx_gui)

# install(TARGETS r64fx_gui DESTINATION lib)
# install(TARGETS r64fx_gui_example DESTINATION bin)
# install(
#     FILES 
#     WindowBase.h
#     Window.h 
#     View.h 
#     Scene.h
#     Widget.h 
#     containers.h 
#     Dummy.h
#     TextLine.h
#     TextEdit.h
#     Icon.h
#     TextEdit.h
#     Config.h
#     MenuItem.h
#     Font.h
#     Message.h
#     
#     DESTINATION
#     include/r64fx/gui
# )
