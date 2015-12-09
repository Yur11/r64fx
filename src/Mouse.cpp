#include "Mouse.hpp"
#include "MouseButtonCodes.hpp"

namespace r64fx{

MouseButton::MouseButton()
: m_code(R64FX_MOUSE_BUTTON_NONE)
{

}


MouseButton MouseButton::none()
{
    return MouseButton(R64FX_MOUSE_BUTTON_NONE);
}


MouseButton MouseButton::left()
{
    return MouseButton(R64FX_MOUSE_BUTTON1);
}


MouseButton MouseButton::middle()
{
    return MouseButton(R64FX_MOUSE_BUTTON2);
}


MouseButton MouseButton::right()
{
    return MouseButton(R64FX_MOUSE_BUTTON3);
}


MouseButton MouseButton::wheelUp()
{
    return MouseButton(R64FX_MOUSE_BUTTON4);
}


MouseButton MouseButton::wheelDown()
{
    return MouseButton(R64FX_MOUSE_BUTTON5);
}

}//namespace r64fx