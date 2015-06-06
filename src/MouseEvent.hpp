#ifndef R64FX_GUI_MOUSE_EVENT_H
#define R64FX_GUI_MOUSE_EVENT_H

#include "Mouse.hpp"
#include "Point.hpp"

namespace r64fx{


class MouseEvent{
   Point<float> m_position;
   unsigned int m_buttons;
    
public:
    MouseEvent(Point<float> position, unsigned int buttons)
    : m_position(position)
    {}
    
    MouseEvent(float x, float y, unsigned int buttons)
    : MouseEvent({x, y}, buttons)
    {}
};

}//namespace r64fx

#endif//R64FX_GUI_MOUSE_EVENT_H