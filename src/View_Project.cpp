#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "StringUtils.hpp"
#include "TextPainter.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

#include "Widget_Knob.hpp"
#include "Widget_Slider.hpp"

namespace r64fx{

Font* g_font = nullptr;

View_Project::View_Project(Widget* parent) : Widget(parent)
{
    if(!g_font)
    {
        g_font = new Font("mono", 14);
    }

    auto knob = new Widget_Knob(48, this);
    knob->setPosition({50, 50});

    auto slider = new Widget_Slider(200, Orientation::Vertical, this);
    slider->setPosition({50, 120});
}


View_Project::~View_Project()
{
    deleteChildren();

    if(g_font)
    {
        delete g_font;
        g_font = nullptr;
    }
}

void View_Project::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    childrenPaintEvent(event);
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        mi -= event->dy();
        if(mi < 0)
            mi = 0;
        repaint();
    }

    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}


}//namespace r64fx
