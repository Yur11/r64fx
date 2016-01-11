#ifndef R64FX_KEYBOARD_MODIFIERS_HPP
#define R64FX_KEYBOARD_MODIFIERS_HPP

namespace r64fx{
namespace Keyboard{

bool LeftShiftDown();
bool LeftCtrlDown();
bool LeftAltDown();
bool RightShiftDown();
bool RightCtrlDown();
bool RightAltDown();

inline bool ShiftDown()
{
    return LeftShiftDown() || RightShiftDown();
}

inline bool CtrlDown()
{
    return LeftCtrlDown() || RightCtrlDown();
}

inline bool AltDown()
{
    return LeftAltDown() || RightAltDown();
}

void trackModifierPress(unsigned int key);

void trackModifierRelease(unsigned int key);

}
}//namespace

#endif//R64FX_KEYBOARD_MODIFIERS_HPP