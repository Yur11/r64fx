#ifndef R64FX_GUI_ACTION_H
#define R64FX_GUI_ACTION_H

#include "Message.h"
#include "Icon.h"
#include "Utf8String.h"

namespace r64fx{

/** @brief An action that can be taken by the user. */
class Action{
    Icon _icon;
    Utf8String _name;
    Message _message;

public:
    Action(Icon icon, Utf8String name, Message message = Message())
    : _icon(icon)
    , _name(name)
    , _message(message)
    {}

    Action(Utf8String name, Message message = Message())
    : _icon(Icon::defaultIcon())
    , _name(name)
    , _message(message)
    {}

    inline void setIcon(Icon icon) { _icon = icon; }
    
    inline Icon icon() const { return _icon; }

    inline void setName(Utf8String name) { _name = name; }

    inline Utf8String name() const { return _name; }

    inline void setMessage(Message message) { _message = message; }

    inline Message message() const { return _message; }

    inline void trigger() { _message.send(this); }
};

}//namespace r64fx

#endif//R64FX_GUI_ACTION_H