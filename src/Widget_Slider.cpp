#include "Widget_Slider.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    Image* img_handle_vert = nullptr;
    Image* img_handle_hori = nullptr;

    void init()
    {
        img_handle_vert = new Image(16, 11, 1);
        for(int x=0; x<16; x++)
        {
            for(int y=0; y<3; y++)
            {
                img_handle_vert->pixel(x, y+1)[0] =
                img_handle_vert->pixel(x, y+7)[0] = 63;
            }

            img_handle_vert->pixel(x, 0)[0]   = 95;
            img_handle_vert->pixel(x, 4)[0]   =
            img_handle_vert->pixel(x, 6)[0]   = 191;
            img_handle_vert->pixel(x, 5)[0]   = 255;
            img_handle_vert->pixel(x, 10)[0]  = 15;
        }

        img_handle_hori = new Image(11, 16, 1);
        for(int y=0; y<16; y++)
        {
            for(int x=0; x<3; x++)
            {
                img_handle_hori->pixel(x+1, y)[0] =
                img_handle_hori->pixel(x+7, y)[0] = 63;
            }

            img_handle_hori->pixel(0,  y)[0]  =
            img_handle_hori->pixel(10, y)[0]  = 111;
            img_handle_hori->pixel(4,  y)[0]  =
            img_handle_hori->pixel(6,  y)[0]  = 191;
            img_handle_hori->pixel(5,  y)[0]  = 255;
        }
    }

    void cleanup()
    {
        if(img_handle_vert)
            delete img_handle_vert;

        if(img_handle_hori)
            delete img_handle_hori;
    }

    int g_slider_count = 0;
}//namespace


Widget_Slider::Widget_Slider(int length, Orientation orientation, Widget* parent)
: Widget(parent)
{
    if(g_slider_count == 0)
    {
        init();
    }
    g_slider_count++;

    setOrientation(orientation);
    if(orientation == Orientation::Vertical)
    {
        setWidth(16);
        setHeight(length);
    }
    else
    {
        setWidth(length);
        setHeight(16);
    }

    onValueChanged(nullptr);
    barVisible(true);
}


Widget_Slider::~Widget_Slider()
{
    g_slider_count--;
    if(g_slider_count == 0)
    {
        cleanup();
    }
}


void Widget_Slider::setValueFromPosition(Point<int> position)
{
    int pos;
    if(orientation() == Orientation::Vertical)
    {
        pos = barLength() - (position.y()- barOffset());
    }
    else
    {
        pos = position.x() - barOffset();
    }

    if(isReversed())
    {
        pos = barLength() - pos;
    }

    float new_value = (float(pos)/float(barLength() - 1)) * valueRange() + minValue();
    setValue(new_value);
    repaint();
}


int Widget_Slider::barLength() const
{
    if(orientation() == Orientation::Vertical)
    {
        return height() - 5;
    }
    else
    {
        return width() - 5;
    }
}


int Widget_Slider::barOffset() const
{
    return 5;
}


bool Widget_Slider::barVisible(bool yes)
{
    if(yes)
        m_flags |= R64FX_SLIDER_BAR_VISIBLE;
    else
        m_flags &= ~R64FX_SLIDER_BAR_VISIBLE;
    return yes;
}


bool Widget_Slider::barVisible() const
{
    return m_flags & R64FX_SLIDER_BAR_VISIBLE;
}


bool Widget_Slider::isFlipped(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_IS_FLIPPED;
    else
        m_flags &= ~R64FX_WIDGET_IS_FLIPPED;
    return yes;
}


bool Widget_Slider::isFlipped() const
{
    return m_flags & R64FX_WIDGET_IS_FLIPPED;
}


bool Widget_Slider::isReversed(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_IS_REVERSED;
    else
        m_flags &= ~R64FX_WIDGET_IS_REVERSED;
    return yes;
}


bool Widget_Slider::isReversed() const
{
    return m_flags & R64FX_WIDGET_IS_REVERSED;
}


void Widget_Slider::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    int pos = ((value() - minValue()) / valueRange()) * (barLength() - 1);

    if(orientation() == Orientation::Vertical)
    {
        if(!isReversed())
            pos = barLength() - pos - 1;

        p->strokeRect({4, 0, 8, pos + 1}, Color(63, 63, 63), Color(95, 95, 95));
        p->strokeRect({4, pos, 8, height() - pos}, Color(63, 96, 127), Color(127, 191, 255));

        p->putImage(img_handle_vert, {0, pos});
    }
    else
    {
        p->putImage(img_handle_hori, {pos, 0});
    }
}


void Widget_Slider::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouseFocus();
        setValueFromPosition(event->position());
    }
}


void Widget_Slider::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(isMouseFocusOwner())
        {
            releaseMouseFocus();
        }
    }
}


void Widget_Slider::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        setValueFromPosition(event->position());
    }
}

}//namespace r64fx
