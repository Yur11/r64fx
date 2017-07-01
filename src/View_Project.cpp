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

    Widget_Slider* slider_vert[4];
    for(int i=0; i<4; i++)
    {
        slider_vert[i] = new Widget_Slider(200, 16, Orientation::Vertical, this);
        slider_vert[i]->setPosition({50 + (slider_vert[i]->width() + 2) * i, 120});
        slider_vert[i]->setMinValue(-1.0f);
        slider_vert[i]->setMaxValue(+1.0f);
    }
    slider_vert[0]->setMinValue(0.0f);
    slider_vert[1]->setMaxValue(0.0f);

    Widget_Slider* slider_hori[4];
    for(int i=0; i<4; i++)
    {
        slider_hori[i] = new Widget_Slider(200, 16, Orientation::Horizontal, this);
        slider_hori[i]->setPosition({50, 350 + (slider_hori[i]->height() + 2) * i});
        slider_hori[i]->setMinValue(-1.0f);
        slider_hori[i]->setMaxValue(+1.0f);
    }
    slider_hori[0]->setMinValue(0.0f);
    slider_hori[1]->setMaxValue(0.0f);
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

    {
        Image atan_table(32, 32, 4);
        gen_atan_table(&atan_table);

        float min_angle = -0.1f * M_PI;
        float max_angle = +0.0f * M_PI;
        Point<int> center(32, 32);
        Image img(64, 64, 1);
        fill(&img, Color(0));
        for(int y=0; y<img.height(); y++)
        {
            for(int x=0; x<img.width(); x++)
            {
                float xyang = atan(x, y, &atan_table);

                if(xyang >= min_angle && xyang <= max_angle)
                {
                    img.pixel(x, y)[0] = 255;
                }
            }
        }
        p->putImage(&img, {300, 100});
    }


    {
        Image radius_table(32, 32, 4);
        gen_radius_table(&radius_table);

        Image img(64, 64, 1);
        fill(&img, Color(0));
        for(int y=0; y<img.height(); y++)
        {
            for(int x=0; x<img.width(); x++)
            {
                float rr = radius(x, y, &radius_table);
                float dd = 16 - rr;
                if(dd < 0.0f)
                    dd = 0.0f;
                else if(dd > 1.0f)
                    dd = 1.0f;

                if(dd >= 0.0f)
                {
                    auto px = img(x, y);
                    px[0] = (unsigned char) (255.0f * dd);
                }
            }
        }
        p->putImage(&img, {400, 100});
    }
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
