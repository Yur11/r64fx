#include "Widget_Button.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "ImageAnimation.hpp"
#include "ImageUtils.hpp"
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
        setState(0);
    }
}


Widget_Button::Widget_Button(ImageAnimation* animation, Widget* parent)
: Widget(parent)
{
    if(!animation)
        return;

    m_animation = animation;
    m_flags &= ~R64FX_WIDGET_OWNS_DATA;
    setSize({m_animation->width(), m_animation->height()});
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


void Widget_Button::setState(int state)
{
    if(state >= 0 && state < m_animation->frameCount())
    {
        m_state = state;
        m_animation->pickFrame(state);
    }
}


int Widget_Button::state()
{
    return m_state;
}


void Widget_Button::pickNextState()
{
    if(!m_animation)
        return;

    m_state++;
    if(m_state >= m_animation->frameCount())
        m_state = 0;
    m_animation->pickFrame(m_state);
}


void Widget_Button::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();
    if(m_animation)
    {
        p->putImage(m_animation, {0, 0});
    }
    Widget::updateEvent(event);
}


void Widget_Button::mousePressEvent(MousePressEvent* event)
{
    m_on_click(this, m_on_click_data);
    Widget::mousePressEvent(event);
}


ImageAnimation* new_button_animation(int width, int height, unsigned char** colors, int nframes)
{
    auto animation = new ImageAnimation(width, height, 4, nframes);
    for(int i=0; i<nframes; i++)
    {
        animation->pickFrame(i);
        fill(animation, colors[i]);
    }
    animation->pickFrame(0);
    return animation;
}

}//namespace r64fx