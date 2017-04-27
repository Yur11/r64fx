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

#include "Atlas.hpp"
#include "Widget_Knob.hpp"

namespace r64fx{

Font* g_font = nullptr;

View_Project::View_Project(Widget* parent) : Widget(parent)
{
    if(!g_font)
    {
        g_font = new Font("mono", 14);
    }

    for(int i=0; i<10; i++)
    {
        auto knob = new Widget_Knob(KnobStyle::Unipolar, 44 + i*4, this);
        knob->setPosition({50 + (44 + i*4)*i, 50});
    }
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

//     Image img(64, 64, 3);
//     fill(&img, Color(255, 255, 255));
//     fill_circle(&img, 1, 2, Color(0, 0), {0, 0}, img.width());
//     fill({&img, {0,             0,              img.width()/2, img.height()/2}},  1, 1, 255);
//     fill({&img, {img.width()/2, img.height()/2, img.width()/2, img.height()/2}},  2, 1, 255);
//     fill({&img, {img.width()/2, 0,              img.width()/2, img.height()/2}},  Color(0, 255, 0));
//     fill({&img, {0,             img.height()/2, img.width()/2, img.height()/2}},  Color(0, 0, 255));
// 
//     for(int i=0; i<8; i++)
//     {
//         p->putImage(&img, {30 + i * 80, 200}, {2, 12, 40, 40}, i);
//     }

    Widget_Knob::debugPaint(p, {50, 150}, 60);
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
