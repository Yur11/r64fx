#ifndef R64FX_GUI_KEY_EVENT_H
#define R64FX_GUI_KEY_EVENT_H

#include "Event.h"

namespace r64fx{
    
class MouseEvent;
    
class KeyEvent : public Event{
    unsigned int _key;

public:
    KeyEvent(Point<float> position, unsigned int buttons, unsigned int key, unsigned int keyboard_modifiers) 
    : Event(position, buttons, keyboard_modifiers)
    , _key(key)
    {}
    
    KeyEvent(float x, float y, unsigned int buttons, unsigned int key, unsigned int keyboard_modifiers)
    : KeyEvent(Point<float>(x, y), buttons, key, keyboard_modifiers)
    {}
    
    inline unsigned int key() const { return _key; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEY_EVENT_H