#ifndef R64FX_WIDGET_TIMELINE_HPP
#define R64FX_WIDGET_TIMELINE_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_Timeline : public Widget{
public:
    Widget_Timeline(Widget* parent = nullptr);

private:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TIMELINE_HPP
