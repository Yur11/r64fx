#include "Mouse.hpp"
#include "MouseButtonCodes.hpp"

namespace r64fx{

MouseButton::MouseButton()
: m_code(R64FX_MOUSE_BUTTON_NONE)
{

}


MouseButton const MouseButton::None()
{
    return MouseButton(R64FX_MOUSE_BUTTON_NONE);
}


MouseButton const MouseButton::Left()
{
    return MouseButton(R64FX_MOUSE_BUTTON1);
}


MouseButton const MouseButton::Middle()
{
    return MouseButton(R64FX_MOUSE_BUTTON2);
}


MouseButton const MouseButton::Right()
{
    return MouseButton(R64FX_MOUSE_BUTTON3);
}


MouseButton const MouseButton::WheelUp()
{
    return MouseButton(R64FX_MOUSE_BUTTON4);
}


MouseButton const MouseButton::WheelDown()
{
    return MouseButton(R64FX_MOUSE_BUTTON5);
}

}//namespace r64fx