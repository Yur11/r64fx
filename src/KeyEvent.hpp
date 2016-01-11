#ifndef R64FX_KEY_EVENT_HPP
#define R64FX_KEY_EVENT_HPP

namespace r64fx{

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
    explicit TextInputEvent(const std::string &text, int key)
    : KeyEvent(key)
    , m_text(text)
    {
    }

    inline std::string text() const { return m_text; }
};

}//namespace r64fx

#endif//R64FX_KEY_EVENT_HPP