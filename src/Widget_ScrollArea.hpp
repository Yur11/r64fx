#ifndef R64FX_WIDGET_SCROLL_AREA_HPP
#define R64FX_WIDGET_SCROLL_AREA_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ScrollArea : public Widget{
    Point<int> m_offset;

public:
    Widget_ScrollArea(Widget* parent = nullptr);

    ~Widget_ScrollArea();

    void setOffset(Point<int> offset);

    Point<int> offset() const;

    virtual Point<int> contentOffset();
};

}//namespace r64fx

#endif//R64FX_WIDGET_SCROLL_AREA_HPP