#include "Action.hpp"

namespace r64fx{

Action::Action(const std::string &caption)
: m_caption(caption)
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

}//namespace r64fx