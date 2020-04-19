#include "Widget_Timeline.hpp"
#include "Painter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

class Widget_TimelineLane : public Widget{
public:
    Widget_TimelineLane(Widget* parent = nullptr) : Widget(parent)
    {
        setHeight(128);
    }

public:
    void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height() - 5}, Color(255, 223, 159));
        childrenPaintEvent(event);
    }
};

Widget_Timeline::Widget_Timeline(Widget* parent) : Widget(parent)
{
    for(int i=0; i<8; i++)
        (void) new Widget_TimelineLane(this);
}

void Widget_Timeline::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(255, 191, 127));
    childrenPaintEvent(event);
}

void Widget_Timeline::resizeEvent(WidgetResizeEvent* event)
{
    Widget_TimelineLane* prev_lane = nullptr;
    for(auto child : *this)
    {
        auto lane = dynamic_cast<Widget_TimelineLane*>(child);
        if(lane)
        {
            lane->setPosition({0, prev_lane ? (prev_lane->y() + prev_lane->height()) : 0});
            lane->setWidth(width());
            prev_lane = lane;
        }
    }
}

}//namespace r64fx
