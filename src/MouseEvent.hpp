#ifndef R64FX_GUI_MOUSE_EVENT_H
#define R64FX_GUI_MOUSE_EVENT_H

#include "Mouse.hpp"
#include "Point.hpp"

namespace r64fx{


class MouseEvent{
   Point<int> m_position;
   unsigned int m_buttons;
    
public:
    MouseEvent(Point<int> position, unsigned int buttons)
    : m_position(position)
    {}
    
    MouseEvent(int x, int y, unsigned int buttons)
    : MouseEvent({x, y}, buttons)
    {}

    inline Point<int> position() const { return m_position; }

    inline int x() const { return m_position.x(); }
    inline int y() const { return m_position.y(); }

    inline unsigned int buttons() const { return m_buttons; }
};

}//namespace r64fx

#endif//R64FX_GUI_MOUSE_EVENT_H