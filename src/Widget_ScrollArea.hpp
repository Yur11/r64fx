#ifndef R64FX_WIDGET_SCROLL_AREA_HPP
#define R64FX_WIDGET_SCROLL_AREA_HPP

#include "Widget.hpp"
#include "Offset.hpp"

namespace r64fx{

class Widget_ScrollArea : public Widget, public OffsetMixin<int>{

public:
    Widget_ScrollArea(Widget* parent = nullptr);

    ~Widget_ScrollArea();
};

}//namespace r64fx

#endif//R64FX_WIDGET_SCROLL_AREA_HPP