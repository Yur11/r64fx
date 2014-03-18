#ifndef R64FX_GUI_ACTION_H
#define R64FX_GUI_ACTION_H

#include "Message.h"
#include "Icon.h"
#include "utf8string/Utf8String.h"

namespace r64fx{

/** @brief An action that can be taken by the user. */
class Action{
    Icon _icon;
    Utf8String _name;
    typedef void* (*Callback)(void*);
    Callback _callback;
    void* _data;
    
public:
    Action(Icon icon, Utf8String name, Callback callback = Action::callbackStub(), void* data = nullptr)
    : _icon(icon)
    , _name(name)
    , _callback(callback)
    , _data(data)
    {}
    
    
    Action(Utf8String name, Callback callback = Action::callbackStub(), void* data = nullptr)
    : _name(name)
    , _callback(callback)
    , _data(data)
    {}

    static Callback callbackStub();

    inline void setIcon(Icon icon) { _icon = icon; }
    
    inline void setIconSize(Size<float> size) { _icon.size = size; }
    
    inline Icon icon() const { return _icon; }

    inline void setName(Utf8String name) { _name = name; }

    inline Utf8String name() const { return _name; }

    inline void trigger() { _callback(_data); }
};

}//namespace r64fx

#endif//R64FX_GUI_ACTION_H