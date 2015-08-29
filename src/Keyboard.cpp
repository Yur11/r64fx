#include "Keyboard.hpp"
#include <map>

using namespace std;

namespace r64fx{

namespace Keyboard{
namespace Key{

map<int, const char*> key2str = {
    { K0, "0" },
    { K1, "1" },
    { K2, "2" },
    { K3, "3" },
    { K4, "4" },
    { K5, "5" },
    { K6, "6" },
    { K7, "7" },
    { K8, "8" },
    { K9, "9" },

    { A, "A" },
    { B, "B" },
    { C, "C" },
    { D, "D" },
    { E, "E" },
    { F, "F" },
    { G, "G" },
    { H, "H" },
    { I, "I" },
    { J, "J" },
    { K, "K" },
    { L, "L" },
    { M, "M" },
    { N, "N" },
    { O, "O" },
    { P, "P" },
    { Q, "Q" },
    { R, "R" },
    { S, "S" },
    { T, "T" },
    { U, "U" },
    { V, "V" },
    { W, "W" },
    { X, "X" },
    { Y, "Y" },
    { Z, "Z" },

    { Space, "Space" },

    { Grave,         "`" },
    { LeftBracket,   "[" },
    { RightBracket,  "]" },
    { Minus,         "-" },
    { Equals,        "=" },
    { Comma,         "," },
    { Period,        "." },
    { Slash,         "/" },
    { Backslash,     "\\" },

    { Backspace, "Backspace" },
    { Return,    "Return" },
    { Escape,    "Escape" },
    { Tab,       "Tab" },
    { Home,      "Home" },
    { End,       "End" },
    { PageUp,    "Page Up" },
    { PageDown,  "Page Down" },
    { Insert,    "Insert" },
    { Delete,    "Delete" },

    { Pause,       "Pause" },
    { PrintScreen, "PrintScreen" },

    { CapsLock,   "Caps Lock" },
    { NumLock,    "Num Lock" },
    { ScrollLock, "Scroll Lock" },

    { LeftShift,  "Left Shift" },
    { RightShift, "Right Shift" },

    { LeftCtrl,  "Left Ctrl" },
    { RightCtrl, "Right Ctrl" },

    { LeftAlt,  "Left Alt" },
    { RightAlt, "Right Alt" },

    { LeftGui,  "Left GUI" },
    { RightGui, "Right GUI" },

    { ContextMenu, "Context Menu" },

    { F1,  "F1" },
    { F2,  "F2" },
    { F3,  "F3" },
    { F4,  "F4" },
    { F5,  "F5" },
    { F6,  "F6" },
    { F7,  "F7" },
    { F8,  "F8" },
    { F9,  "F9" },
    { F10, "F10" },
    { F11, "F11" },
    { F12, "F12" },
    { F13, "F13" },
    { F14, "F14" },
    { F15, "F15" },
    { F16, "F16" },
    { F17, "F17" },
    { F18, "F18" },
    { F19, "F19" },
    { F20, "F20" },
    { F21, "F21" },
    { F22, "F22" },
    { F23, "F23" },
    { F24, "F24" },

    { Up,    "Up" },
    { Down,  "Down" },
    { Left,  "Left" },
    { Right, "Right" },

    { KP0, "Key Pad 0" },
    { KP1, "Key Pad 1" },
    { KP2, "Key Pad 2" },
    { KP3, "Key Pad 3" },
    { KP4, "Key Pad 4" },
    { KP5, "Key Pad 5" },
    { KP6, "Key Pad 6" },
    { KP7, "Key Pad 7" },
    { KP8, "Key Pad 8" },
    { KP9, "Key Pad 9" },

    { KP_Enter,    "Key Pad Enter" },
    { KP_Plus,     "Key Pad +" },
    { KP_Minus,    "Key Pad -" },
    { KP_Multiply, "Key Pad *" },
    { KP_Divide,   "Key Pad /" },
    { KP_Period,   "Key Pad ." }
};


const char* toString(int key)
{
    auto it = key2str.find(key);
    if(it == key2str.end()) return "";
    return it->second;
}

}//namespace Key
}//namespace Keyboard

}//namespace r64fx
