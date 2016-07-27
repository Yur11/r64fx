#include "Action.hpp"

namespace r64fx{

Action::Action(const std::string &caption)
: m_caption(caption)
{

}


Action::~Action()
{

}


void Action::setCaption(const std::string &caption)
{
    m_caption = caption;
}


std::string Action::caption() const
{
    return m_caption;
}


void Action::exec()
{

}


void CallbackAction::exec()
{
    m_callback(m_arg, this);
}

}//namespace r64fx