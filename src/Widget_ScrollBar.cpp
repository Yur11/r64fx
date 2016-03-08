#include "Widget_ScrollBar.hpp"
#include "Image.hpp"
#include "Painter.hpp"
#include <cmath>

#include <iostream>

using namespace std;

namespace r64fx{

int g_scroll_bar_width = 20;

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
                    img_button_right -> pixel(j, i)[0]                          =
                    img_button_up    -> pixel(i, j)[0];
                }
            }
        }
    }

    void cleanup_images()
    {

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


Widget_ScrollBar_Vertical::Widget_ScrollBar_Vertical(Widget* parent)
: Widget_ScrollBar(parent)
{
    setWidth(g_scroll_bar_width);
}


void Widget_ScrollBar_Vertical::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char fg[4] = {127, 127, 127, 0};
    static unsigned char bg[4] = {127, 180, 255, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int button_size    = width();
    int bar_length     = height() - button_size * 2;
    int handle_length  = bar_length * m_ratio;
    int handle_offset  = (bar_length - handle_length) * m_handle_position;
    Rect<int> handle_rect(0, button_size + handle_offset, width(), handle_length);

    p->fillRect(handle_rect, fg);

    unsigned char* colors[1] = {fg};
    p->blendColors({0, 0                     }, colors, img_button_up);
    p->blendColors({0, height() - button_size}, colors, img_button_down);

    Widget::reconfigureEvent(event);
}


void Widget_ScrollBar_Vertical::mousePressEvent(MousePressEvent* event)
{
    Widget_ScrollBar::mousePressEvent(event);
}


void Widget_ScrollBar_Vertical::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()) || event->dy() == 0)
        return;

    int button_size  = width();
    int bar_length   = height() - button_size * 2;
    int length       = bar_length - bar_length * m_ratio;
    float step       = float(event->dy()) / float(length);
    setHandlePosition(handlePosition() + step);
    update();
}



Widget_ScrollBar_Horizontal::Widget_ScrollBar_Horizontal(Widget* parent)
: Widget_ScrollBar(parent)
{
    setHeight(g_scroll_bar_width);
}


void Widget_ScrollBar_Horizontal::reconfigureEvent(ReconfigureEvent* event)
{
//     static unsigned char color[4] = {255, 0, 0, 0};
//
//     auto p = event->painter();
//     Rect<int> handle_rect;
//
//     int handle_length = width() * m_ratio;
//     int handle_offset = (width() - handle_length) * m_handle_position;
//     handle_rect = {handle_offset, 0, handle_length, height()};
//
//     p->fillRect(handle_rect, color);
//
//     Widget::reconfigureEvent(event);
}


void Widget_ScrollBar_Horizontal::mousePressEvent(MousePressEvent* event)
{
//     Widget_ScrollBar::mousePressEvent(event);
}


void Widget_ScrollBar_Horizontal::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()))
        return;
}

}//namespace r64fx