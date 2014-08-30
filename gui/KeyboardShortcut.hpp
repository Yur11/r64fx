#ifndef R64FX_GUI_KEYBOARD_SHORTCUT_H
#define R64FX_GUI_KEYBOARD_SHORTCUT_H

#include "Keyboard.hpp"
#include <string>

namespace r64fx{
    
class KeyboardShortcut{
    unsigned long long bits;
    
public:
    KeyboardShortcut(unsigned int key, unsigned int mods)
    {
        bits = key | ((unsigned long long)mods) << 32;
    }
    
    inline operator unsigned long long()
    {
        return bits;
    }
    
    inline unsigned int key() const
    {
        return (unsigned int) bits;
    }
    
    inline unsigned int mods() const
    {
        return (unsigned int) (bits >> 32);
    }
    
    std::string toStr();
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEYBOARD_SHORTCUT_H