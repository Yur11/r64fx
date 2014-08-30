#ifndef R64FX_GUI_ACTION_H
#define R64FX_GUI_ACTION_H

#include <string>

namespace r64fx{

/** @brief An action that can be taken by the user. */
struct Action{
    std::string name;
    std::string caption;
    typedef void* (*Callback)(void*);
    Callback callback;
    void* data;
    
    Action(std::string name, std::string caption, Callback callback = Action::callbackStub(), void* data = nullptr)
    : name(name)
    , caption(caption)
    , callback(callback)
    , data(data)
    {}

    static Callback callbackStub();

    inline void trigger() { callback(data); }
};

}//namespace r64fx

#endif//R64FX_GUI_ACTION_H