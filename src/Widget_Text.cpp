#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "Font.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Program.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

Widget_Text::Widget_Text(std::string* textptr, Font* font, Widget* parent)
: Widget(parent)
{
    setText(textptr);
    setFont(font);
}


Widget_Text::Widget_Text(const std::string &text, Font* font, Widget* parent)
: Widget(parent)
{
    setText(text);
    setFont(font);
}


Widget_Text::Widget_Text(const std::string &text, Widget* parent)
: Widget(parent)
{
    setText(text);
    setFont(nullptr);
}


Widget_Text::Widget_Text(Widget* parent)
: Widget(parent)
{
    setText(nullptr);
    setFont(nullptr);
}


Widget_Text::~Widget_Text()
{

}


void Widget_Text::setText(std::string *textptr)
{
    if(ownsText())
    {
        delete m_text_painter.text;
    }

    if(textptr)
    {
        m_text_painter.text = textptr;
        m_flags &= ~R64FX_WIDGET_OWNS_TEXT;
    }
    else
    {
        m_text_painter.text = new std::string;
        m_flags |= R64FX_WIDGET_OWNS_TEXT;
    }
}


void Widget_Text::setText(const std::string &text)
{
    if(!m_text_painter.text)
    {
        m_text_painter.text = new std::string(text);
        m_flags |= R64FX_WIDGET_OWNS_TEXT;
    }
    else
    {
        m_text_painter.text->assign(text);
    }
}


void Widget_Text::setFont(Font* font)
{
    if(ownsFont())
    {
        delete m_text_painter.font;
    }

    if(font)
    {
        m_text_painter.font = font;
        m_flags &= ~R64FX_WIDGET_OWNS_FONT;
    }
    else
    {
        m_text_painter.font = new Font;
        m_flags |= R64FX_WIDGET_OWNS_FONT;
    }
}


void Widget_Text::setFont(std::string font_name)
{
    if(ownsFont())
    {
        delete m_text_painter.font;
    }

    m_text_painter.font = new Font(font_name);
    m_flags |= R64FX_WIDGET_OWNS_FONT;
}


bool Widget_Text::ownsText() const
{
    return m_flags & R64FX_WIDGET_OWNS_TEXT;
}


bool Widget_Text::ownsFont() const
{
    return m_flags & R64FX_WIDGET_OWNS_FONT;
}


void Widget_Text::reflow(TextWrap wrap_mode)
{
    reflow(wrap_mode, width());
}


void Widget_Text::reflow(TextWrap wrap_mode, int width)
{
    m_text_painter.reflow(wrap_mode, width);
}


void Widget_Text::resizeToText()
{
    setSize(m_text_painter.textSize());
}


void Widget_Text::reconfigureEvent(ReconfigureEvent* event)
{
    if(m_text_painter.image)
    {
        delete m_text_painter.image;
    }
    m_text_painter.image = new Image(width(), height(), 1);
    unsigned char px = 0;
    m_text_painter.image->fill(&px);

    m_text_painter.paint(m_text_painter.image, {10, 10});
    draw_rect(
        m_text_painter.image,
        {255, 255, 255},
        intersection(
            Rect<int>(10, 10, textSize().width(), textSize().height()),
            Rect<int>(0, 0, width(), height())
        )
    );

    auto painter = event->painter();
    painter->blendColors({0, 0, 0}, {255, 255, 255}, m_text_painter.image);
    Widget::reconfigureEvent(event);
}


void Widget_Text::focusInEvent()
{
    startTextInput();
    update();
}


void Widget_Text::focusOutEvent()
{
    stopTextInput();
    update();
}


void Widget_Text::resizeEvent(ResizeEvent* event)
{
    reflow(TextWrap::None, width() - 20);
}


void Widget_Text::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
//     m_text_painter.putCursor(event->position());
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


void Widget_Text::textInputEvent(TextInputEvent* event)
{
    if(event->key() == Keyboard::Key::Escape)
    {
        removeFocus();
    }
    else if(event->key() == Keyboard::Key::Home)
    {
//         m_text_painter.homeCursor();
    }
    else if(event->key() == Keyboard::Key::End)
    {
//         m_text_painter.endCursor();
    }
    else if(event->key() == Keyboard::Key::Delete)
    {
//         m_text_painter.deleteAfterCursor();
    }
    else if(event->key() == Keyboard::Key::Backspace)
    {
//         m_text_painter.deleteBeforeCursor();
    }
    else
    {
        m_text_painter.inputUtf8(event->text());
    }

    update();
}


void Widget_Text::closeEvent()
{
    Program::quit();
}

}//namespace r64fx