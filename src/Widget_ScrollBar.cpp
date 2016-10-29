#include "Widget_ScrollBar.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include <cmath>

#include <iostream>

using namespace std;

namespace r64fx{

int g_scroll_bar_width = 15;
int g_scroll_bar_count = 0;

namespace{
    Image* img_button_up     = nullptr;
    Image* img_button_down   = nullptr;
    Image* img_button_left   = nullptr;
    Image* img_button_right  = nullptr;

    void init_images()
    {
        img_button_up     = new Image;
        img_button_down   = new Image;
        img_button_left   = new Image;
        img_button_right  = new Image;

        draw_triangles(g_scroll_bar_width, img_button_up, img_button_down, img_button_left, img_button_right);
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
    if(g_scroll_bar_count == 0)
    {
        init_images();
    }
    g_scroll_bar_count++;
}


Widget_ScrollBar::~Widget_ScrollBar()
{
    g_scroll_bar_count--;
    if(g_scroll_bar_count == 0)
    {
        cleanup_images();
    }
}


void Widget_ScrollBar::setRatio(float ratio)
{
    m_ratio = ratio;
}

float Widget_ScrollBar::ratio() const
{
    return m_ratio;
}


void Widget_ScrollBar::setHandlePosition(float position, bool notify)
{
    m_handle_position = position;
    if(m_handle_position < 0.0f)
        m_handle_position = 0.0f;
    else if(m_handle_position > 1.0f)
        m_handle_position = 1.0f;

    if(notify)
        m_position_changed(this, m_position_changed_data);
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


void Widget_ScrollBar::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(isMouseFocusOwner())
    {
        releaseMouseFocus();
    }
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


void Widget_ScrollBar_Vertical::paintEvent(WidgetPaintEvent* event)
{
    static unsigned char fg[4] = {0, 0, 127, 0};
    static unsigned char bg[4] = {0, 127, 0, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int handle_offset  = (barLength() - handleLength()) * m_handle_position;
    Rect<int> handle_rect(0, g_scroll_bar_width + handle_offset, width(), handleLength());
    p->fillRect(handle_rect, fg);

    unsigned char* colors[1] = {fg};
//     p->blendColors({0, 0                            }, colors, img_button_up);
//     p->blendColors({0, height() - g_scroll_bar_width}, colors, img_button_down);

    childrenPaintEvent(event);
}


void Widget_ScrollBar_Vertical::mousePressEvent(MousePressEvent* event)
{
    if(event->y() < g_scroll_bar_width)
    {
        setHandlePosition(handlePosition() - 0.1f, true);
        repaint();
    }
    else if(event->y() > (height() - g_scroll_bar_width))
    {
        setHandlePosition(handlePosition() + 0.1f, true);
        repaint();
    }
    else
    {
        childrenMousePressEvent(event);
        grabMouseFocus();
    }
}


void Widget_ScrollBar_Vertical::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()) || event->dy() == 0)
        return;

    int length = barLength() - handleLength();
    if(length > 0)
    {
        float step = float(event->dy()) / float(length);
        setHandlePosition(handlePosition() + step, true);
        repaint();
    }
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


void Widget_ScrollBar_Horizontal::paintEvent(WidgetPaintEvent* event)
{
    static unsigned char fg[4] = {0, 0, 127, 0};
    static unsigned char bg[4] = {0, 127, 0, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int handle_offset  = (barLength() - handleLength()) * m_handle_position;
    Rect<int> handle_rect(g_scroll_bar_width + handle_offset, 0, handleLength(), height());
    p->fillRect(handle_rect, fg);

    unsigned char* colors[1] = {fg};
//     p->blendColors({0, 0                           }, colors, img_button_left);
//     p->blendColors({width() - g_scroll_bar_width, 0}, colors, img_button_right);

    childrenPaintEvent(event);
}


void Widget_ScrollBar_Horizontal::mousePressEvent(MousePressEvent* event)
{
    if(event->x() < g_scroll_bar_width)
    {
        setHandlePosition(handlePosition() - 0.1, true);
        repaint();
    }
    else if(event->x() > (width() - g_scroll_bar_width))
    {
        setHandlePosition(handlePosition() + 0.1, true);
        repaint();
    }
    else
    {
        childrenMousePressEvent(event);
        grabMouseFocus();
    }
}


void Widget_ScrollBar_Horizontal::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()))
        return;

    int length = barLength() - handleLength();
    if(length > 0)
    {
        float step       = float(event->dx()) / float(length);
        setHandlePosition(handlePosition() + step, true);
        repaint();
    }
}

}//namespace r64fx
