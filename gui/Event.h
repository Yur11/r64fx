#ifndef R64FX_GUI_EVENT_H
#define R64FX_GUI_EVENT_H

#include "Keyboard.h"

namespace r64fx{

class WindowBase;
// class View;
class Scene;

/** @brief Base class for keyboard, mouse and other events. 
 
    Event class containes information about event origins, 
    that can be usefull for the recipient object.
    As the event is created by the window and is delivered
    down through the splitted views, to the scene 
    and further down the widget tree it is filled with data.
    This data consits of:
    
        - A pointer to the window that orinated the event.
        
        - A pointer to the scene that recieved the event.
 */
class Event{
    friend class WindowBase;
//     friend class View;
    friend class Scene;
    WindowBase* origin_window = nullptr;
    Scene* _scene = nullptr;
    
    unsigned int _keyboard_modifiers = Keyboard::Modifier::None;
    
public:
    bool has_been_handled = false;
    
    Event(unsigned int keyboard_modifiers = Keyboard::Modifier::None) : _keyboard_modifiers(keyboard_modifiers) {}
    
    inline WindowBase* originWindow() const { return this->origin_window; }

    inline Scene* scene() const { return _scene; }
    
    inline int keyboardModifiers() { return _keyboard_modifiers; }
    
    inline void setKeyboardModifiers(unsigned int keyboard_modifiers) { _keyboard_modifiers = keyboard_modifiers; }
    
private:
    inline void setOriginWindow(WindowBase* window) { this->origin_window = window; }

    inline void setScene(Scene* scene) { _scene = scene; }
    
};

}//namespace r64fx

#endif//R64FX_GUI_EVENT_H