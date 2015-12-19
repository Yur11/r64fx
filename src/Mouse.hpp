#ifndef R64FX_MOUSE_HPP
#define R64FX_MOUSE_HPP

#include "Point.hpp"

namespace r64fx{

class MouseButton{
    unsigned int m_code;

public:
    explicit MouseButton(unsigned int code)
    : m_code(code)
    {

    }

    MouseButton();

    inline unsigned int code() const { return m_code; }

    inline operator bool() const { return m_code; }

    inline MouseButton &operator=(const MouseButton &other)
    {
        m_code = other.code();
        return *this;
    }

    static const MouseButton None();

    static const MouseButton Left();

    static const MouseButton Middle();

    static const MouseButton Right();

    static const MouseButton WheelUp();

    static const MouseButton WheelDown();
};


inline bool operator==(MouseButton a, MouseButton b)
{
    return a.code() == b.code();
}


inline bool operator!=(MouseButton a, MouseButton b)
{
    return !operator==(a, b);
}


inline MouseButton operator|(MouseButton a, MouseButton b)
{
    return MouseButton(a.code() | b.code());
}


inline MouseButton operator&(MouseButton a, MouseButton b)
{
    return MouseButton(a.code() & b.code());
}


inline MouseButton operator~(MouseButton a)
{
    return MouseButton(~a.code());
}


inline MouseButton &operator|=(MouseButton &a, MouseButton b)
{
    a = a | b;
    return a;
}


inline MouseButton &operator&=(MouseButton &a, MouseButton b)
{
    a = a & b;
    return a;
}


class MouseEvent{
    Point<int>  m_position;
    MouseButton m_button;

public:
    bool handled = false;

    explicit MouseEvent(int x, int y, MouseButton button)
    : m_position{x, y}
    , m_button(button)
    {

    }

    inline void setPosition(Point<int> position) { m_position = position; }

    inline Point<int> position() const { return m_position; }

    inline int x() const { return m_position.x(); }
    inline int y() const { return m_position.y(); }

    inline MouseButton button() const { return m_button; }
};


class MousePressEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};


class MouseReleaseEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};


class MouseMoveEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};

}//namespace r64fx

#endif//R64FX_MOUSE_HPP
