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

}//namespace r64fx

#endif//R64FX_MOUSE_HPP
