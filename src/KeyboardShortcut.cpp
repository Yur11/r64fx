#include "KeyboardShortcut.hpp"

using namespace std;

namespace r64fx{
    
string KeyboardShortcut::toStr()
{
    string str;
    
    if(mods() & (Keyboard::Modifier::LeftCtrl | Keyboard::Modifier::RightCtrl))
    {
        str += "Ctrl + ";
    }
    
    if(mods() & (Keyboard::Modifier::LeftShift | Keyboard::Modifier::RightShift))
    {
        str += "Shift + ";
    }
    
    if(mods() & (Keyboard::Modifier::LeftAlt | Keyboard::Modifier::RightAlt))
    {
        str += "Alt + ";
    }
    
    str += Keyboard::Key::str(key());
    
    return str;
}
    
}//namespace r64fx