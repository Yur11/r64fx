#include "Widget_Timeline.hpp"
#include "Painter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

class Widget_TimelineClip : public Widget{
public:
    Widget_TimelineClip(Widget* parent = nullptr) : Widget(parent)
    {
        setWidth(1024);
    }

private:
    void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->strokeRect({0, 0, width(), height()}, Color(63, 32, 32), Color(255, 127, 127), 5);
        childrenPaintEvent(event);
    }
};

class Widget_TimelineLane : public Widget{
public:
    Widget_TimelineLane(Widget* parent = nullptr) : Widget(parent)
    {
        setHeight(256);

        //REMOVE ME
        for(int i=0; i<3; i++)
            (void) new Widget_TimelineClip(this);
    }

private:
    void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height() - 5}, Color(255, 223, 159));
        childrenPaintEvent(event);
    }

    void resizeEvent(WidgetResizeEvent* event)
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
