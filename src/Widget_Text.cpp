#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "TextEditingUtils.hpp"
#include "UndoRedoUtils.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

#include <iostream>

#define m_text_painter ((TextPainter*)m[0])

#define m_undo_redo_chain ((UndoRedoChain*)m[1])

using namespace std;

namespace r64fx{

namespace{
    string g_selection_text = "";
    string g_clipboard_text = "";
}//namespace


Widget_Text::Widget_Text(const std::string &text, Font* font, Widget* parent)
: Widget(parent)
{
    m[0] = new TextPainter;
    setFont(font);
    setText(text);
    initUndoRedoChain();
}


Widget_Text::Widget_Text(const std::string &text, Widget* parent)
: Widget(parent)
{
    m[0] = new TextPainter;
    setFont(nullptr);
    setText(text);
    initUndoRedoChain();
}


Widget_Text::Widget_Text(Font* font, Widget* parent)
{
    m[0] = new TextPainter;
    setFont(font);
    setText("");
    initUndoRedoChain();
}


Widget_Text::Widget_Text(Widget* parent)
: Widget(parent)
{
    m[0] = new TextPainter;
    setFont(nullptr);
    initUndoRedoChain();
}


void Widget_Text::initUndoRedoChain()
{
    m[1] = new UndoRedoChain;
}


Widget_Text::~Widget_Text()
{
    delete m_text_painter;
}


void Widget_Text::setText(const std::string &text)
{
    m_text_painter->clear();
    m_text_painter->insertText(text);
}


void Widget_Text::insertText(const std::string &text)
{
    insert_text(m_text_painter, m_undo_redo_chain, text);
}


void Widget_Text::deleteAtCursorPosition(bool backspace)
{
    delete_text_at_cursor(m_text_painter, m_undo_redo_chain, backspace);
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


void Widget_Text::paintEvent(WidgetPaintEvent* event)
{
    if(m_image)
    {
        delete m_image;
    }
    m_image = new Image(width(), height(), 4);
    if(isMouseFocusOwner())
    {
        fill(m_image, Color(255, 127, 127, 0));
    }
    else
    {
        fill(m_image, Color(255, 255, 255, 0));
    }

    m_text_painter->paintSelectionBackground(
        m_image, Color(148, 202, 239), {paddingLeft(), paddingTop()}
    );

    m_text_painter->paintText(
        m_image, Color(0, 0, 0), Color(0, 0, 0), {paddingLeft(), paddingTop()}
    );

    for(int y=0; y<m_text_painter->font->height(); y++)
    {
        auto cursor_pos = m_text_painter->findCursorCoords(m_text_painter->cursorPosition());
        int xx = cursor_pos.x() + paddingLeft();
        int yy = cursor_pos.y() + paddingTop() + y;

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
    painter->putImage(m_image, {0, 0});

    childrenPaintEvent(event);
}


void Widget_Text::focusInEvent()
{
    cout << "Widget_Text::focusInEvent()\n";
    startTextInput();
    repaint();
}


void Widget_Text::focusOutEvent()
{
    cout << "Widget_Text::focusOutEvent()\n";
    stopTextInput();
    repaint();
}


void Widget_Text::resizeEvent(WidgetResizeEvent* event)
{
    m_text_painter->setReflowWidth(event->width() - paddingLeft() - paddingRight());
    m_text_painter->reflow();
    m_text_painter->realign();
}


void Widget_Text::mousePressEvent(MousePressEvent* event)
{
    grabKeyboardFocus();

    if(event->button() == MouseButton::Right())
    {
    }
    else if(event->button() == MouseButton::Left() || event->button() == MouseButton::Middle())
    {
        auto tcp = m_text_painter->findCursorPosition(
            event->position() - Point<int>(paddingLeft(), paddingTop())
        );
        m_text_painter->setCursorPosition(tcp);
        m_text_painter->setSelectionStart(tcp);
        m_text_painter->setSelectionEnd(tcp);
        m_text_painter->updateSelection();
        if(event->button() == MouseButton::Middle())
        {
            requestClipboardMetadata(ClipboardMode::Selection);
        }
    }

    repaint();
}


void Widget_Text::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(m_text_painter->hasSelection())
    {
        g_selection_text = m_text_painter->selectionText();
        anounceClipboardData("text/plain", ClipboardMode::Selection);
    }
    repaint();
}


void Widget_Text::mouseMoveEvent(MouseMoveEvent* event)
{
    if(pressedButtons() & MouseButton::Left())
    {
        auto tcp = m_text_painter->findCursorPosition(
            event->position() - Point<int>(paddingLeft(), paddingTop())
        );
        m_text_painter->setCursorPosition(tcp);
        m_text_painter->setSelectionEnd(tcp);
        m_text_painter->updateSelection();
        repaint();
    }
}


void Widget_Text::keyPressEvent(KeyPressEvent* event)
{

}


void Widget_Text::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void Widget_Text::textInputEvent(TextInputEvent* event)
{
    auto tp     = m_text_painter;
    auto uc     = m_undo_redo_chain;
    auto key    = event->key();
    const auto &text  = event->text();
    bool touched_selection = false;

    if(event->key() == Keyboard::Key::Escape)
    {
        releaseKeyboardFocus();
    }
    else if(
        undo_redo(uc, key) ||
        cursor_hori(tp, key, &touched_selection) ||
        cursor_vert(tp, key, &touched_selection) ||
        select_all(tp, key, &touched_selection)  ||
        delete_text(tp, key, uc, &touched_selection)
    ){}
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::C)
    {
        if(tp->hasSelection())
        {
            g_clipboard_text = tp->selectionText();
            anounceClipboardData("text/plain", ClipboardMode::Clipboard);
        }
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::X)
    {
        if(tp->hasSelection())
        {
            g_clipboard_text = tp->selectionText();
            anounceClipboardData("text/plain", ClipboardMode::Clipboard);
            deleteAtCursorPosition(false);
        }
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::V)
    {
        requestClipboardMetadata(ClipboardMode::Clipboard);
    }
    else if(key == Keyboard::Key::Return)
    {
        insertText("\n");
    }
    else if(!text.empty())
    {
        insertText(text);
    }

    if(tp->hasSelection() && touched_selection)
    {
        g_selection_text = m_text_painter->selectionText();
        anounceClipboardData("text/plain", ClipboardMode::Selection);
    }

    repaint();
}


void Widget_Text::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{
    cout << event->type().name() << "\n";
    if(event->mode() != ClipboardMode::Bad && event->data() != nullptr && event->size() > 0)
    {
        string text((const char*)event->data(), event->size());
        insertText(text);
        repaint();
    }
}


void Widget_Text::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{
    event->transmit((void*)g_selection_text.c_str(), g_selection_text.size());
}


void Widget_Text::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{
    if(event->has("text/plain"))
    {
        requestClipboardData("text/plain", event->mode());
    }
}

}//namespace r64fx
