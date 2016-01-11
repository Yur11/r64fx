#ifndef R64FX_KEYBOARD_MODIFIERS_HPP
#define R64FX_KEYBOARD_MODIFIERS_HPP

#include "KeyboardModifiers.hpp"
#include "Keyboard.hpp"

namespace{
    bool g_left_shift_down  = false;
    bool g_left_ctrl_down   = false;
    bool g_left_alt_down    = false;
    bool g_right_shift_down = false;
    bool g_right_ctrl_down  = false;
    bool g_right_alt_down   = false;
}


namespace r64fx{
namespace Keyboard{

bool LeftShiftDown()
{
    return g_left_shift_down;
}

bool LeftCtrlDown()
{
    return g_left_ctrl_down;
}

bool LeftAltDown()
{
    return g_left_alt_down;
}

bool RightShiftDown()
{
    return g_right_shift_down;
}

bool RightCtrlDown()
{
    return g_right_ctrl_down;
}

bool RightAltDown()
{
    return g_right_alt_down;
}


void trackModifierPress(unsigned int key)
{
    switch(key)
    {
        case Key::LeftShift:
            g_left_shift_down  = true;
        break;

        case Key::LeftCtrl:
            g_left_ctrl_down   = true;
        break;

        case Key::LeftAlt:
            g_left_alt_down    = true;
        break;

        case Key::RightShift:
            g_right_shift_down = true;
        break;

        case Key::RightCtrl:
            g_right_ctrl_down  = true;
        break;

        case Key::RightAlt:
            g_right_alt_down   = true;
        break;

        default:
            break;
    }
}

void trackModifierRelease(unsigned int key)
{
    switch(key)
    {
        case Key::LeftShift:
            g_left_shift_down  = false;
        break;

        case Key::LeftCtrl:
            g_left_ctrl_down   = false;
        break;

        case Key::LeftAlt:
            g_left_alt_down    = false;
        break;

        case Key::RightShift:
            g_right_shift_down = false;
        break;

        case Key::RightCtrl:
            g_right_ctrl_down  = false;
        break;

        case Key::RightAlt:
            g_right_alt_down   = false;
        break;

        default:
            break;
    }
}

}
}//namespace r64fx

#endif//R64FX_KEYBOARD_MODIFIERS_HPP