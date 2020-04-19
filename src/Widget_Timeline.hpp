#ifndef R64FX_WIDGET_TIMELINE_HPP
#define R64FX_WIDGET_TIMELINE_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_TimelineClip : public Widget{
public:
    Widget_TimelineClip(Widget* parent = nullptr);

private:
    void paintEvent(WidgetPaintEvent* event);
};


class Widget_TimelineLane : public Widget{
public:
    Widget_TimelineLane(Widget* parent = nullptr);

private:
    void paintEvent(WidgetPaintEvent* event);

    void resizeEvent(WidgetResizeEvent* event);
};

class Widget_Timeline : public Widget{
public:
    Widget_Timeline(Widget* parent = nullptr);

private:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TIMELINE_HPP
