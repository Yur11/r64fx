#ifndef R64FX_GUI_MOUSE_EVENT_H
#define R64FX_GUI_MOUSE_EVENT_H

#include "Mouse.hpp"
#include "Event.hpp"

namespace r64fx{

class Window;
class SplittableView;
class View;
class Scene;
class Widget;


/** @brief Mouse movement and button event data structure. 
 
    Mouse events are usually created by the window and delivered down through the views, the scene
    and the widget tree.
 */
class MouseEvent : public Event{
   
public:
    MouseEvent(Point<float> position, unsigned int buttons = Mouse::Button::None, unsigned int keyboard_modifiers = Keyboard::Modifier::None)
    : Event(position, buttons, keyboard_modifiers)
    {}
    
    MouseEvent(float x = 0.0, float y = 0.0, unsigned int buttons = Mouse::Button::None, unsigned int keyboard_modifiers = Keyboard::Modifier::None)
    : MouseEvent(Point<float>(x, y), buttons, keyboard_modifiers)
    {}
};

}//namespace r64fx

#endif//R64FX_GUI_MOUSE_EVENT_H