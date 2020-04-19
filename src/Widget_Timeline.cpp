#include "Widget_Timeline.hpp"
#include "Painter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{


Widget_TimelineClip::Widget_TimelineClip(Widget* parent) : Widget(parent)
{
    setWidth(1024);
}

Widget_TimelineLane::Widget_TimelineLane(Widget* parent) : Widget(parent)
{
    setHeight(256);

    //REMOVE ME
    for(int i=0; i<3; i++)
        (void) new Widget_TimelineClip(this);
}

Widget_Timeline::Widget_Timeline(Widget* parent) : Widget(parent)
{
    for(int i=0; i<8; i++)
        (void) new Widget_TimelineLane(this);
}


void Widget_TimelineClip::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->strokeRect({0, 0, width(), height()}, Color(63, 32, 32), Color(255, 127, 127), 5);
    childrenPaintEvent(event);
}

void Widget_TimelineLane::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height() - 5}, Color(255, 223, 159));
    childrenPaintEvent(event);
}

void Widget_Timeline::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(255, 191, 127));
    childrenPaintEvent(event);
}


void Widget_TimelineLane::resizeEvent(WidgetResizeEvent* event)
{
    for(auto child : *this)
    {
        auto clip = dynamic_cast<Widget_TimelineClip*>(child);
        if(clip)
        {
            clip->setPosition({0, 0});
            clip->setHeight(height() - 5);
        }
    }
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
