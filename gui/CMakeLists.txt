#r64fx gui

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
target_link_libraries(r64fx_gui 
    r64fx_serialize 
    r64fx_utf8string 
    r64fx_paths 
    SDL2 
    GL 
    GLU 
    ftgl 
    png
)

