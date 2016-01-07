#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "Font.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Program.hpp"
#include "TextPainter.hpp"

#include <iostream>

#define m_text_painter ((TextPainter*)m)

using namespace std;

namespace r64fx{

Widget_Text::Widget_Text(std::string* textptr, Font* font, Widget* parent)
: Widget(parent)
{
    m = new TextPainter;
    setText(textptr);
    setFont(font);
}


Widget_Text::Widget_Text(const std::string &text, Font* font, Widget* parent)
: Widget(parent)
{
    m = new TextPainter;
    setText(text);
    setFont(font);
}


Widget_Text::Widget_Text(const std::string &text, Widget* parent)
: Widget(parent)
{
    m = new TextPainter;
    setText(text);
    setFont(nullptr);
}


Widget_Text::Widget_Text(Widget* parent)
: Widget(parent)
{
    m = new TextPainter;
    setText(nullptr);
    setFont(nullptr);
}


Widget_Text::~Widget_Text()
{
    delete m_text_painter;
}


void Widget_Text::setText(std::string *textptr)
{
    if(ownsText())
    {
        delete m_text;
    }

    if(textptr)
    {
        m_text = textptr;
        m_flags &= ~R64FX_WIDGET_OWNS_TEXT;
    }
    else
    {
        m_text = new std::string;
        m_flags |= R64FX_WIDGET_OWNS_TEXT;
    }
}


void Widget_Text::setText(const std::string &text)
{
    if(!m_text)
    {
        m_text = new std::string(text);
        m_flags |= R64FX_WIDGET_OWNS_TEXT;
    }
    else
    {
        m_text->assign(text);
    }
}


void Widget_Text::setFont(Font* font)
{
    if(ownsFont())
    {
        delete m_text_painter->font;
    }

    if(font)
    {
        m_text_painter->font = font;
        m_flags &= ~R64FX_WIDGET_OWNS_FONT;
    }
    else
    {
        m_text_painter->font = new Font;
        m_flags |= R64FX_WIDGET_OWNS_FONT;
    }
}


void Widget_Text::setFont(std::string font_name)
{
    if(ownsFont())
    {
        delete m_text_painter->font;
    }

    m_text_painter->font = new Font(font_name);
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


void Widget_Text::setTextWrap(TextWrap text_wrap)
{
    m_text_painter->setTextWrap(text_wrap);
}


TextWrap Widget_Text::textWrap() const
{
    return m_text_painter->textWrap();
}


void Widget_Text::setTextAlignment(TextAlignment alignment)
{
    m_text_painter->setTextAlignment(alignment);
}


TextAlignment Widget_Text::textAlignment() const
{
    return m_text_painter->textAlignment();
}


void Widget_Text::resizeToText()
{
    setSize(m_text_painter->textSize());
}


void Widget_Text::reconfigureEvent(ReconfigureEvent* event)
{
    if(m_image)
    {
        delete m_image;
    }
    m_image = new Image(width(), height(), 4);
    fill(m_image, {255, 255, 255, 0});

    m_text_painter->paintSelectionBackground(m_image, {148, 202, 239}, {10, 10});
    m_text_painter->paintText(m_image, {0, 0, 0}, {0, 0, 0}, {10, 10});
//     draw_rect(
//         m_image,
//         {0, 0, 0},
//         intersection(
//             Rect<int>(10, 10, m_text_painter->textSize().width(), m_text_painter->textSize().height()),
//             Rect<int>(0, 0, width(), height())
//         )
//     );

    for(int y=0; y<m_text_painter->font->height(); y++)
    {
        auto cursor_pos = m_text_painter->findCursorCoords(m_text_painter->cursorPosition());
        int xx = cursor_pos.x() + 10;
        int yy = cursor_pos.y() + 10 + y;

        if(xx >= 0 &&
           xx < m_image->width() &&
           yy >=0 &&
           yy < m_image->height())
        {
            unsigned char px[4] = {0, 0, 0, 0};
            m_image->setPixel(xx, yy, px);
        }
    }

    auto painter = event->painter();
    painter->putImage(m_image);

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
    m_text_painter->setReflowWidth(event->width() - 20);
    m_text_painter->reflow(*m_text);
    m_text_painter->reallign();
}


void Widget_Text::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
    if(event->button() == MouseButton::Left())
    {
        auto tcp = m_text_painter->findCursorPosition(event->position() - Point<int>(10, 10));
        m_text_painter->setCursorPosition(tcp);
        m_text_painter->setSelectionStart(tcp);
        m_text_painter->setSelectionEnd(tcp);
        m_text_painter->updateSelection();
    }
    update();
}


void Widget_Text::mouseReleaseEvent(MouseReleaseEvent* event)
{
    Widget::mouseReleaseEvent(event);
}


void Widget_Text::mouseMoveEvent(MouseMoveEvent* event)
{
    if(pressedButtons() & MouseButton::Left())
    {
        auto tcp = m_text_painter->findCursorPosition(event->position() - Point<int>(10, 10));
        m_text_painter->setCursorPosition(tcp);
        m_text_painter->setSelectionEnd(tcp);
        m_text_painter->updateSelection();
        update();
    }
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
    else if(event->key() == Keyboard::Key::Up)
    {
        m_text_painter->moveCursorUp();
    }
    else if(event->key() == Keyboard::Key::Down)
    {
        m_text_painter->moveCursorDown();
    }
    else if(event->key() == Keyboard::Key::Left)
    {
        m_text_painter->moveCursorLeft();
    }
    else if(event->key() == Keyboard::Key::Right)
    {
        m_text_painter->moveCursorRight();
    }
    else if(event->key() == Keyboard::Key::Home)
    {
        m_text_painter->homeCursor();
    }
    else if(event->key() == Keyboard::Key::End)
    {
        m_text_painter->endCursor();
    }
    else if(event->key() == Keyboard::Key::Delete)
    {
        m_text_painter->deleteAfterCursor(*m_text);
    }
    else if(event->key() == Keyboard::Key::Backspace)
    {
        m_text_painter->deleteBeforeCursor(*m_text);
    }
    else
    {
        m_text_painter->inputUtf8(event->text(), *m_text);
    }

    update();
}


void Widget_Text::closeEvent()
{
    Program::quit();
}

}//namespace r64fx