#include "Widget_Button.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "ImageAnimation.hpp"
#include "TextPainter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    void on_click_stub(Widget_Button*, void*) {}
}


Widget_Button::Widget_Button(Widget* parent)
: Widget(parent)
, m_on_click(on_click_stub)
{
    setSize({10, 10});
}


Widget_Button::Widget_Button(std::string text, std::string font_name, Widget* parent)
: Widget(parent)
, m_on_click(on_click_stub)
{
    m_animation = new ImageAnimation;

    Font font(font_name);
    resize_image_and_draw_text(m_animation, text, TextWrap::None, &font);
    m_flags |= R64FX_WIDGET_OWNS_DATA;

    if(m_animation)
    {
        setSize({m_animation->width(), m_animation->height()});
    }
}


Widget_Button::~Widget_Button()
{
    if(m_flags & R64FX_WIDGET_OWNS_DATA && m_animation)
    {
        delete m_animation;
    }
}


void Widget_Button::onClick(void (*callback)(Widget_Button* button, void* data), void* data)
{
    if(callback)
        m_on_click = callback;
    else
        m_on_click = on_click_stub;
    m_on_click_data = data;
}


void Widget_Button::setState(int frame)
{
    if(frame < m_animation->frameCount() && frame >= 0)
        m_animation->pickFrame(frame);
}


void Widget_Button::reconfigureEvent(ReconfigureEvent* event)
{
    unsigned char bg[4] = {0, 0, 0, 0};
    unsigned char fg[4] = {255, 255, 255, 255};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg);
    unsigned char* colors[1] = {fg};
    p->blendColors({0, 0}, colors, m_animation);
    Widget::reconfigureEvent(event);
}


void Widget_Button::mousePressEvent(MousePressEvent* event)
{
    m_on_click(this, m_on_click_data);
    Widget::mousePressEvent(event);
}

}//namespace r64fx