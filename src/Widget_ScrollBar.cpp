#include "Widget_ScrollBar.hpp"
#include "Image.hpp"
#include "Painter.hpp"
#include <cmath>

#include <iostream>

using namespace std;

namespace r64fx{

int g_scroll_bar_width = 16;

namespace{
    Image* img_button_up     = nullptr;
    Image* img_button_down   = nullptr;
    Image* img_button_left   = nullptr;
    Image* img_button_right  = nullptr;

    void init_images_if_needed()
    {
        if(!img_button_up)
        {
            auto img = new Image(g_scroll_bar_width, g_scroll_bar_width);
            img_button_up = img;

            int half_width = g_scroll_bar_width / 2;
            int triangle_height = sqrt(g_scroll_bar_width * g_scroll_bar_width - half_width * half_width);
            float slope = float(half_width) / float(triangle_height);
            for(int y=0; y<triangle_height; y++)
            {
                float threshold = y * slope;
                for(int x=0; x<half_width; x++)
                {
                    float distance = half_width - x - 1;
                    float diff = distance - threshold;
                    unsigned char px;
                    if(diff < 0.0f)
                        px = 255;
                    else if(diff < 1.0f)
                        px = 255 - (unsigned char)(diff * 255.0f);
                    else
                        px = 0;
                    img->pixel(x, y)[0] = img->pixel(g_scroll_bar_width - x - 1, y)[0] = px;
                }
            }

            img_button_down  = new Image(g_scroll_bar_width, g_scroll_bar_width);
            img_button_left  = new Image(g_scroll_bar_width, g_scroll_bar_width);
            img_button_right = new Image(g_scroll_bar_width, g_scroll_bar_width);

            for(int i=0; i<g_scroll_bar_width; i++)
            {
                for(int j=0; j<g_scroll_bar_width; j++)
                {
                    img_button_down  -> pixel(i, g_scroll_bar_width - j - 1)[0] =
                    img_button_left  -> pixel(j, g_scroll_bar_width - i - 1)[0] =
                    img_button_right -> pixel(g_scroll_bar_width - j - 1, i)[0] =
                    img_button_up    -> pixel(i, j)[0];
                }
            }
        }
    }

    void cleanup_images()
    {
        if(img_button_up)
            delete img_button_up;

        if(img_button_down)
            delete img_button_down;

        if(img_button_left)
            delete img_button_left;

        if(img_button_right)
            delete img_button_right;

        img_button_up = img_button_down = img_button_left = img_button_right = nullptr;
    }

    void position_changed_stub(Widget_ScrollBar*, void*) {}
}

Widget_ScrollBar::Widget_ScrollBar(Widget* parent)
: Widget(parent)
, m_position_changed(position_changed_stub)
{
    init_images_if_needed();
}


void Widget_ScrollBar::setRatio(float ratio)
{
    m_ratio = ratio;
}

float Widget_ScrollBar::ratio() const
{
    return m_ratio;
}


void Widget_ScrollBar::setHandlePosition(float position)
{
    m_handle_position = position;
    if(m_handle_position < 0.0f)
        m_handle_position = 0.0f;
    else if(m_handle_position > 1.0f)
        m_handle_position = 1.0f;
}


float Widget_ScrollBar::handlePosition() const
{
    return m_handle_position;
}


void Widget_ScrollBar::onPositionChanged(void(*callback)(Widget_ScrollBar* scroll_bar, void* data), void* data)
{
    if(callback)
        m_position_changed = callback;
    else
        m_position_changed = position_changed_stub;
    m_position_changed_data = data;
}


int Widget_ScrollBar::handleLength()
{
    return barLength() * ratio();
}


Widget_ScrollBar_Vertical::Widget_ScrollBar_Vertical(Widget* parent)
: Widget_ScrollBar(parent)
{
    setWidth(g_scroll_bar_width);
}


int Widget_ScrollBar_Vertical::barLength()
{
    return height() - g_scroll_bar_width * 2;
}


void Widget_ScrollBar_Vertical::paintEvent(PaintEvent* event)
{
    static unsigned char fg[4] = {127, 127, 127, 0};
    static unsigned char bg[4] = {127, 180, 255, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int handle_offset  = (barLength() - handleLength()) * m_handle_position;
    Rect<int> handle_rect(0, g_scroll_bar_width + handle_offset, width(), handleLength());
    p->fillRect(handle_rect, fg);

    unsigned char* colors[1] = {fg};
    p->blendColors({0, 0                            }, colors, img_button_up);
    p->blendColors({0, height() - g_scroll_bar_width}, colors, img_button_down);

    Widget::paintEvent(event);
}


void Widget_ScrollBar_Vertical::mousePressEvent(MousePressEvent* event)
{
    if(event->y() < g_scroll_bar_width)
    {
        setHandlePosition(handlePosition() - 0.1f);
        repaint();
    }
    else if(event->y() > (height() - g_scroll_bar_width))
    {
        setHandlePosition(handlePosition() + 0.1f);
        repaint();
    }
    else
    {
        Widget_ScrollBar::mousePressEvent(event);
    }
}


void Widget_ScrollBar_Vertical::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()) || event->dy() == 0)
        return;

    int length = barLength() - handleLength();
    float step = float(event->dy()) / float(length);
    setHandlePosition(handlePosition() + step);
    repaint();
}



Widget_ScrollBar_Horizontal::Widget_ScrollBar_Horizontal(Widget* parent)
: Widget_ScrollBar(parent)
{
    setHeight(g_scroll_bar_width);
}


int Widget_ScrollBar_Horizontal::barLength()
{
    return width() - g_scroll_bar_width * 2;
}


void Widget_ScrollBar_Horizontal::paintEvent(PaintEvent* event)
{
    static unsigned char fg[4] = {127, 127, 127, 0};
    static unsigned char bg[4] = {127, 180, 255, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int handle_offset  = (barLength() - handleLength()) * m_handle_position;
    Rect<int> handle_rect(g_scroll_bar_width + handle_offset, 0, handleLength(), height());
    p->fillRect(handle_rect, fg);

    unsigned char* colors[1] = {fg};
    p->blendColors({0, 0                           }, colors, img_button_left);
    p->blendColors({width() - g_scroll_bar_width, 0}, colors, img_button_right);

    Widget::paintEvent(event);
}


void Widget_ScrollBar_Horizontal::mousePressEvent(MousePressEvent* event)
{
    if(event->x() < g_scroll_bar_width)
    {
        setHandlePosition(handlePosition() - 0.1);
        repaint();
    }
    else if(event->x() > (width() - g_scroll_bar_width))
    {
        setHandlePosition(handlePosition() + 0.1);
        repaint();
    }
    else
    {
        Widget_ScrollBar::mousePressEvent(event);
    }
}


void Widget_ScrollBar_Horizontal::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()))
        return;

    int length       = barLength() - handleLength();
    float step       = float(event->dx()) / float(length);
    setHandlePosition(handlePosition() + step);
    repaint();
}

}//namespace r64fx