#ifndef R64FX_GUI_KEY_EVENT_H
#define R64FX_GUI_KEY_EVENT_H

#include "Event.h"

namespace r64fx{
    
class MouseEvent;
    
class KeyEvent : public Event{
    unsigned int _key;
    unsigned int _buttons;
    unsigned int _modifiers;

public:
    /** @brief Previous mouse event. 
     
        This is needed primarily to make the current mouse position available to key event handlers.
        The WindowBase is responsible for setting this pointer.
     */
    MouseEvent* mouse_event;
    
    KeyEvent(unsigned int key, unsigned int buttons, unsigned int modifiers, MouseEvent* mouse_event) 
    : _key(key)
    , _buttons(buttons)
    , _modifiers(modifiers)
    , mouse_event(mouse_event) 
    {}
    
    inline unsigned int key() const { return _key; }
    inline unsigned int buttons() const { return _buttons; }
    inline unsigned int modifiers() const { return _modifiers; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEY_EVENT_H