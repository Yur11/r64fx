#include "Widget_Slider.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    constexpr int g_handle_size = 15;

    Image* img_triangle_up     = nullptr;
    Image* img_triangle_down   = nullptr;
    Image* img_triangle_left   = nullptr;
    Image* img_triangle_right  = nullptr;

    void init()
    {
        img_triangle_up     = new Image;
        img_triangle_down   = new Image;
        img_triangle_left   = new Image;
        img_triangle_right  = new Image;

        draw_triangles(g_handle_size, img_triangle_up, img_triangle_down, img_triangle_left, img_triangle_right);
    }

    void cleanup()
    {
        if(img_triangle_up)
            delete img_triangle_up;

        if(img_triangle_down)
            delete img_triangle_down;

        if(img_triangle_left)
            delete img_triangle_left;

        if(img_triangle_right)
            delete img_triangle_right;

        img_triangle_up = img_triangle_down = img_triangle_left = img_triangle_right = nullptr;
    }

    int g_slider_count = 0;

    void on_value_changed_stub(void* arg, Widget_Slider* slider, float value) {}
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
        setWidth(g_handle_size + 2);
        setHeight(length);
    }
    else
    {
        setWidth(length);
        setHeight(g_handle_size + 2);
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


void Widget_Slider::setMinValue(float value)
{
    m_min_value = value;
}


float Widget_Slider::minValue() const
{
    return m_min_value;
}


void Widget_Slider::setMaxValue(float value)
{
    m_max_value = value;
}


float Widget_Slider::maxValue() const
{
    return m_max_value;
}


float Widget_Slider::valueRange() const
{
    return maxValue() - minValue();
}


void Widget_Slider::setValue(float value)
{
    m_value = value;
    if(value < m_min_value)
        m_value = m_min_value;
    else if(value > m_max_value)
        m_value = m_max_value;
}


void Widget_Slider::setValue(Point<int> position)
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


float Widget_Slider::value() const
{
    return m_value;
}


int Widget_Slider::barLength() const
{
    if(orientation() == Orientation::Vertical)
    {
        return height() - g_handle_size;
    }
    else
    {
        return width() - g_handle_size;
    }
}


int Widget_Slider::barOffset() const
{
    return g_handle_size >> 1;
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


void Widget_Slider::onValueChanged(void (on_value_changed)(void* arg, Widget_Slider* slider, float value), void* arg)
{
    if(on_value_changed)
    {
        m_on_value_changed = on_value_changed;
    }
    else
    {
        m_on_value_changed = on_value_changed_stub;
    }
    m_on_value_changed_arg = arg;
}


void Widget_Slider::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char bg[4] = {127, 127, 127, 0};
    unsigned char* colors[1] = {black};

    p->fillRect({0, 0, width(), height()}, bg);

    int pos = ((value() - minValue()) / valueRange()) * (barLength() - 1);

    if(orientation() == Orientation::Vertical)
    {
        if(!isReversed())
            pos = barLength() - pos - 1;

        int bar_x;
        int handle_x;
        Image* handle_img;
        if(isFlipped())
        {
            bar_x = g_handle_size;
            handle_x = 0;
            handle_img = img_triangle_right;
        }
        else
        {
            bar_x = 0;
            handle_x = 3;
            handle_img = img_triangle_left;
        }

        if(barVisible())
        {
            p->fillRect({bar_x, barOffset(), 2, barLength()}, black);
        }
        p->blendColors({handle_x, pos}, colors, handle_img);
    }
    else
    {
        if(isReversed())
            pos = barLength() - pos - 1;

        int bar_y;
        int handle_y;
        Image* handle_img;
        if(isFlipped())
        {
            bar_y = 0;
            handle_y = 2;
            handle_img = img_triangle_up;
        }
        else
        {
            bar_y = g_handle_size;
            handle_y = 0;
            handle_img = img_triangle_down;
        }

        if(barVisible())
        {
            p->fillRect({barOffset(), bar_y, barLength(), 2}, black);
        }
        p->blendColors({pos, handle_y}, colors, handle_img);
    }
}


void Widget_Slider::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouse();
        setValue(event->position());
    }
}


void Widget_Slider::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(isMouseGrabber())
        {
            ungrabMouse();
        }
    }
}


void Widget_Slider::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        setValue(event->position());
        m_on_value_changed(m_on_value_changed_arg, this, value());
    }
}

}//namespace r64fx