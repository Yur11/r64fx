#ifndef R64FX_CLIPBOARD_EVENT_HPP
#define R64FX_CLIPBOARD_EVENT_HPP

#include <string>

namespace r64fx{

class ClipboardEvent{
public:
    enum class Type{
        Paste,
        Selection
    };

private:
    std::string m_text;
    Type        m_type;

public:
    ClipboardEvent(const std::string &text, Type type)
    : m_text(text)
    , m_type(type)
    {}

    inline std::string text() const { return m_text; }

    inline Type type() const { return m_type; }
};

}//namespace r64x

#endif//R64FX_CLIPBOARD_EVENT_HPP