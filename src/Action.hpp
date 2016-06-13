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

}//namespace r64fx

#endif//R64FX_ACTION_HPP