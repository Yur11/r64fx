#ifndef R64FX_KEYBOARD_HPP
#define R64FX_KEYBOARD_HPP

#if defined R64FX_USE_X11
#include "KeyboardX11.hpp"
#endif

#include <string>

namespace r64fx{

namespace Keyboard{
namespace Key{
    const char* toString(int key);
}//namespace Key

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

}//namespace Keyboard


class KeyEvent{
    int m_key;

public:
    explicit KeyEvent(int key)
    : m_key(key)
    {}

    inline int key() const { return m_key; }
};


class KeyPressEvent : public KeyEvent{
public:
    explicit KeyPressEvent(int key) : KeyEvent(key) {}
};


class KeyReleaseEvent : public KeyEvent{
public:
    explicit KeyReleaseEvent(int key) : KeyEvent(key) {}
};


class TextInputEvent : public KeyEvent{
    std::string m_text;

public:
    explicit TextInputEvent(const std::string &text, int key);

    inline std::string text() const { return m_text; }
};

}//namespace r64fx

#endif//R64FX_GUI_KEYBOARD_HPP