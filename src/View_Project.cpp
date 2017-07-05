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


class RingSectorPainter{
    Image m_table;

public:
    RingSectorPainter(int size)
    : m_table((size >> 1), (size >> 1), 8)
    {
#ifdef R64FX_DEBUG
        assert(size > 0);
        assert((size & 1) == 0);
#endif//R64FX_DEBUG
        gen_atan_table(&m_table, 0);
        gen_radius_table(&m_table, 1);
    }

    void paint(ImgPos dst, float min_angle, float max_angle, float outer_radius, float inner_radius = 0)
    {
        float atanval = 0.0f;
        float radiusval = 0.0f;
        int w = (m_table.width() << 1);
        int h = (m_table.height() << 1);

        for(int y=0; y<h; y++)
        {
            for(int x=0; x<w; x++)
            {
                atan_and_radius(x, y, &m_table, atanval, radiusval);

                if(atanval >= min_angle && atanval <= max_angle)
                {
                    auto dstpx = dst.img->pixel(x + dst.pos.x(), y + dst.pos.y());

                    float outer = outer_radius - radiusval;
                    if(outer < 0.0f)
                        outer = 0.0f;
                    else if(outer > 1.0f)
                        outer = 1.0f;

                    float inner = radiusval - inner_radius;
                    if(inner < 0.0f)
                        inner = 0.0f;
                    else if(inner > 1.0f)
                        inner = 1.0f;

                    dstpx[0] = (unsigned char) (255.0f * outer * inner);
                }
            }
        }
    }
};


void View_Project::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    childrenPaintEvent(event);

    {
        Image img(64, 64, 1);
        fill(&img, Color(0));
        RingSectorPainter rsp(64);
        rsp.paint(&img, +0.35f*M_PI, +0.70f*M_PI, 31.0f, 27.0f);
        p->putImage(&img, {500, 100});
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
