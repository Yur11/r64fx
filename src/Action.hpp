#ifndef R64FX_ACTION_HPP
#define R64FX_ACTION_HPP

#include <string>

namespace r64fx{

class Action{
    std::string m_caption;

public:
    Action(const std::string &caption);

    virtual ~Action();

    void setCaption(const std::string &caption);

    std::string caption() const;

    virtual void exec();
};


class CallbackAction : public Action{
    void (*m_callback)(void* arg, CallbackAction* action) = nullptr;
    void* m_arg = nullptr;

public:
    CallbackAction(const std::string &caption, void (*callback)(void* arg, CallbackAction* action), void* arg)
    : Action(caption)
    , m_callback(callback)
    , m_arg(arg)
    {

    }

    virtual void exec();
};

}//namespace r64fx

#endif//R64FX_ACTION_HPP