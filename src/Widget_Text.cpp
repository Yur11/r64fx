#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include <new>

namespace r64fx{

Widget_Text::Widget_Text(std::string* textptr, Font* font, Widget* parent)
: Widget(parent)
{
    if(textptr)
        setText(textptr);
    else
        setText("");
    setFont(font);
}


Widget_Text::Widget_Text(const std::string &text, Font* font, Widget* parent)
: Widget(parent)
{
    setText(text);
    setFont(font);
}


Widget_Text::Widget_Text(Widget* parent)
: Widget(parent)
{
    setText("");
}


Widget_Text::~Widget_Text()
{
    if(Widget_Text::ownsData())
    {
        delete m_text;
    }
}


void Widget_Text::free()
{
    if(m_text && Widget_Text::ownsData())
    {
        delete m_text;
    }
}


bool Widget_Text::ownsData() const
{
    return m_flags & R64FX_WIDGET_OWNS_DATA;
}


void Widget_Text::setText(std::string *text)
{
    free();
    m_text = text;
    m_flags &= ~R64FX_WIDGET_OWNS_DATA;
}


void Widget_Text::setText(const std::string &text)
{
    free();
    m_text = new (std::nothrow) std::string(text);
    m_flags |= R64FX_WIDGET_OWNS_DATA;
}


void Widget_Text::setFont(Font* font)
{
    m_font = font;
}


void Widget_Text::reconfigureEvent(ReconfigureEvent* event)
{
    auto painter = event->painter();
    painter->fillRect({255, 0, 0}, rect());
    Widget::reconfigureEvent(event);
}


void Widget_Text::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
}


void Widget_Text::mouseReleaseEvent(MouseReleaseEvent* event)
{
    Widget::mouseReleaseEvent(event);
}


void Widget_Text::mouseMoveEvent(MouseMoveEvent* event)
{
    Widget::mouseMoveEvent(event);
}


void Widget_Text::keyPressEvent(KeyPressEvent* event)
{
    Widget::keyPressEvent(event);
}


void Widget_Text::keyReleaseEvent(KeyReleaseEvent* event)
{
    Widget::keyReleaseEvent(event);
}

}//namespace r64fx