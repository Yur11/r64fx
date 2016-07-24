#include "Widget_Slider.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"

namespace r64fx{

namespace{
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

        draw_triangles(15, img_triangle_up, img_triangle_down, img_triangle_left, img_triangle_right);
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
        setWidth(img_triangle_left->width() + 2);
        setHeight(length);
    }
    else
    {
        setWidth(length);
        setHeight(img_triangle_down->height() + 2);
    }
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
}


float Widget_Slider::value() const
{
    return m_value;
}


void Widget_Slider::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char bg[4] = {127, 127, 127, 0};
    unsigned char* colors[1] = {black};

    p->fillRect({0, 0, width(), height()}, bg);

    if(orientation() == Orientation::Vertical)
    {
        int offset = img_triangle_left->height()/2;
        int length = height() - img_triangle_left->height();
        int pos = ((value() - minValue()) / valueRange()) * length - offset;

        p->fillRect({0, offset, 2, length}, black);
        p->blendColors({3, pos}, colors, img_triangle_left);
    }
    else
    {
        int offset = img_triangle_down->width()/2;
        int length = width() - img_triangle_down->width();
        int pos = ((value() - minValue()) / valueRange()) * length - offset;

        p->fillRect({offset, img_triangle_down->height(), length, 2}, black);
        p->blendColors({pos, 0}, colors, img_triangle_down);
    }
}


void Widget_Slider::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouse();
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
        int pos;
        int length;
        if(orientation() == Orientation::Vertical)
        {
            pos = event->y();
            length = height() - img_triangle_down->height();
        }
        else
        {
            pos = event->x();
            length = width() - img_triangle_left->width();
        }

        float new_value = (float(pos)/float(length)) * (valueRange()) + minValue();
        setValue(new_value);
        repaint();
    }
}

}//namespace r64fx