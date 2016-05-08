#include "Widget_ScrollArea.hpp"

namespace r64fx{

Widget_ScrollArea::Widget_ScrollArea(Widget* parent) : Widget(parent)
{

}


Widget_ScrollArea::~Widget_ScrollArea()
{

}


void Widget_ScrollArea::setOffset(Point<int> offset)
{
    m_offset = offset;
}


Point<int> Widget_ScrollArea::offset() const
{
    return m_offset;
}


Point<int> Widget_ScrollArea::contentOffset()
{
    return offset();
}

}//namespace r64fx