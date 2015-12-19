#ifndef R64FX_KEYBOARD_HPP
#define R64FX_KEYBOARD_HPP

#if defined R64FX_USE_SDL2
#include "KeyboardSDL2.hpp"

#elif defined R64FX_USE_X11
#include "KeyboardX11.hpp"

#endif //R64FX_USE_SDL2

namespace r64fx{

namespace Keyboard{
namespace Key{

const char* toString(int key);

}//namespace Key
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
    char m_utf8[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    unsigned int m_size;

public:
    explicit TextInputEvent(char* utf8, unsigned int size, int key);

    inline const char* utf8() { return m_utf8; }

    inline unsigned int size() const { return m_size; }
};

}//namespace r64fx

#endif//R64FX_GUI_KEYBOARD_HPP