#ifndef R64FX_GUI_KEY_EVENT_H
#define R64FX_GUI_KEY_EVENT_H

#include "Event.h"

namespace r64fx{
    
class KeyEvent : public Event{
    unsigned int _key;
    unsigned int _buttons;
    unsigned int _modifiers;

public:
    KeyEvent(unsigned int key, unsigned int buttons, unsigned int modifiers) 
    : _key(key)
    , _buttons(buttons)
    , _modifiers(modifiers) {}
    
    inline unsigned int key() const { return _key; }
    inline unsigned int buttons() const { return _buttons; }
    inline unsigned int modifiers() const { return _modifiers; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEY_EVENT_H