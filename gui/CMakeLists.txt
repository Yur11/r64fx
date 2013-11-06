#r64fx gui

add_library(r64fx_gui
    SHARED
    Window.cpp
    SDL2Window.cpp
    View.cpp
    Scene.cpp
    Widget.cpp
    containers.cpp
    Dummy.cpp
    TextLine.cpp
    TextEdit.cpp
    Icon.cpp
    Texture.cpp
    TextureAtlas.cpp
    Config.cpp
    Action.cpp
    Menu.cpp
    Font.cpp
    Message.cpp
    Keyboard.cpp
    KeyboardShortcut.cpp
    bezier.cpp
    rotations.cpp
) 
target_link_libraries(r64fx_gui 
    r64fx_utf8string
    r64fx_gc
    SDL2 
    GL 
    GLU 
    ftgl 
    png
)

