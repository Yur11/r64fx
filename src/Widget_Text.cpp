#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "Font.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Mouse.hpp"
#include "Program.hpp"
#include "TextPainter.hpp"
#include "UndoRedoChain.hpp"

#include <iostream>

#define m_text_painter ((TextPainter*)m[0])

#define m_undo_redo_chain ((UndoRedoChain*)m[1])

using namespace std;

namespace r64fx{

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


Widget_Text::Widget_Text(Widget* parent)
: Widget(parent)
{
    m[0] = new TextPainter;
    setFont(nullptr);
    initUndoRedoChain();
}


namespace{

class UndoRedoItem_TextAdded : public UndoRedoItem{
    GlyphString m_glyphs;
    int         m_cursor_position;

public:
    UndoRedoItem_TextAdded(const GlyphString &glyphs, int cursor_position)
    : m_glyphs(glyphs)
    , m_cursor_position(cursor_position)
    {}

    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;

        tp->setSelectionStart(
            tp->glyphIndexToCursorPosition(m_cursor_position)
        );

        tp->setSelectionEnd(
            tp->glyphIndexToCursorPosition(m_cursor_position + m_glyphs.size())
        );

        tp->deleteSelection();
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;
        tp->setCursorPosition(tp->glyphIndexToCursorPosition(m_cursor_position));
        tp->insertText(m_glyphs);
    }
};


class UndoRedoItem_TextDeleted : public UndoRedoItem{
    GlyphString m_glyphs;
    int         m_cursor_position;
    int         m_selection_start;
    int         m_selection_end;

public:
    UndoRedoItem_TextDeleted(const GlyphString &glyphs, int cursor_position, int selection_start, int selection_end)
    : m_glyphs(glyphs)
    , m_cursor_position(cursor_position)
    , m_selection_start(selection_start)
    , m_selection_end(selection_end)
    {}

    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;
        tp->setCursorPosition(tp->glyphIndexToCursorPosition(m_cursor_position));
        tp->insertText(m_glyphs);
        if(m_selection_start != m_selection_end)
        {
            tp->setSelectionStart(tp->glyphIndexToCursorPosition(m_selection_start));
            tp->setSelectionEnd(tp->glyphIndexToCursorPosition(m_selection_end));
            tp->updateSelection();
        }
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;
        tp->setCursorPosition(tp->glyphIndexToCursorPosition(m_cursor_position));
        if(m_selection_start != m_selection_end)
        {
            tp->setSelectionStart(tp->glyphIndexToCursorPosition(m_selection_start));
            tp->setSelectionEnd(tp->glyphIndexToCursorPosition(m_selection_end));
        }
        tp->deleteAfterCursor();
    }
};


class UndoRedoItem_TextReplaced : public UndoRedoItem{
    GlyphString m_removed_glyphs;
    GlyphString m_added_glyphs;
    int         m_cursor_position;

public:
    UndoRedoItem_TextReplaced(const GlyphString &removed_glyphs, const GlyphString &added_glyphs, int cursor_position)
    : m_removed_glyphs(removed_glyphs)
    , m_added_glyphs(added_glyphs)
    , m_cursor_position(cursor_position)
    {}

    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;

        tp->setSelectionStart(
            tp->glyphIndexToCursorPosition(m_cursor_position)
        );

        tp->setSelectionEnd(
            tp->glyphIndexToCursorPosition(m_cursor_position + m_added_glyphs.size())
        );

        tp->deleteSelection();

        tp->setCursorPosition(tp->glyphIndexToCursorPosition(m_cursor_position));
        tp->insertText(m_removed_glyphs);

        tp->setSelectionStart(
            tp->glyphIndexToCursorPosition(m_cursor_position)
        );

        tp->setSelectionEnd(
            tp->glyphIndexToCursorPosition(m_cursor_position + m_removed_glyphs.size())
        );

        tp->updateSelection();
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;

        tp->setSelectionStart(
            tp->glyphIndexToCursorPosition(m_cursor_position)
        );

        tp->setSelectionEnd(
            tp->glyphIndexToCursorPosition(m_cursor_position + m_removed_glyphs.size())
        );

        tp->deleteSelection();

        tp->setCursorPosition(tp->glyphIndexToCursorPosition(m_cursor_position));
        tp->insertText(m_added_glyphs);
    }
};

}//namespace


void Widget_Text::initUndoRedoChain()
{
    m[1] = new UndoRedoChain;
    m_undo_redo_chain->setData(m_text_painter);
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
    auto tp = m_text_painter;
    auto uc = m_undo_redo_chain;

    int cursor_position = tp->cursorPositionToGlyphIndex(tp->cursorPosition());

    GlyphString removed_glyphs, added_glyphs;
    tp->insertText(text, &removed_glyphs, &added_glyphs);

    if(uc)
    {
        if(removed_glyphs.empty())
        {
            uc->addItem(new UndoRedoItem_TextAdded(added_glyphs, cursor_position));
        }
        else
        {
            uc->addItem(new UndoRedoItem_TextReplaced(removed_glyphs, added_glyphs, cursor_position));
        }
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
    m_text_painter->reflow();
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
    auto tp     = m_text_painter;
    auto uc     = m_undo_redo_chain;
    auto key    = event->key();
    const auto &text  = event->text();

    if(event->key() == Keyboard::Key::Escape)
    {
        removeFocus();
    }
    else if(Keyboard::CtrlDown() && event->key() == Keyboard::Key::Z)
    {
        cout << "undo: " << uc->index() << "\n";
        uc->undo();
    }
    else if(Keyboard::CtrlDown() && (event->key() == Keyboard::Key::Y))
    {
        cout << "redo: " << uc->index() << "\n";
        uc->redo();
    }
    else if(key == Keyboard::Key::Up)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectUp();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorUp();
        }
    }
    else if(key == Keyboard::Key::Down)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectDown();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorDown();
        }
    }
    else if(key == Keyboard::Key::Left)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectLeft();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorLeft();
        }
    }
    else if(key == Keyboard::Key::Right)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectRight();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorRight();
        }
    }
    else if(key == Keyboard::Key::Home)
    {
        if(Keyboard::ShiftDown())
        {
            tp->homeSelection();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->homeCursor();
        }
    }
    else if(key == Keyboard::Key::End)
    {
        if(Keyboard::ShiftDown())
        {
            tp->endSelection();
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->endCursor();
        }
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::A)
    {
        tp->selectAll();
    }
    else if(key == Keyboard::Key::Delete)
    {
        int selection_start = tp->cursorPositionToGlyphIndex(tp->selectionStart());
        int selection_end   = tp->cursorPositionToGlyphIndex(tp->selectionEnd());
        int cursor_pos      = tp->cursorPositionToGlyphIndex(tp->cursorPosition());

        GlyphString glyphs;
        tp->deleteAfterCursor(&glyphs);

        uc->addItem(new UndoRedoItem_TextDeleted(
            glyphs, cursor_pos, selection_start, selection_end
        ));

        cout << uc->size() << "\n";
    }
    else if(key == Keyboard::Key::Backspace)
    {
        int selection_start = tp->cursorPositionToGlyphIndex(tp->selectionStart());
        int selection_end   = tp->cursorPositionToGlyphIndex(tp->selectionEnd());

        GlyphString glyphs;
        tp->deleteBeforeCursor(&glyphs);

        int cursor_pos = tp->cursorPositionToGlyphIndex(tp->cursorPosition());

        uc->addItem(new UndoRedoItem_TextDeleted(
            glyphs, cursor_pos, selection_start, selection_end
        ));

        cout << uc->size() << "\n";
    }
    else if(key == Keyboard::Key::Return)
    {
        insertText("\n");
    }
    else if(!text.empty())
    {
        insertText(text);
    }

    update();
}


void Widget_Text::closeEvent()
{
    Program::quit();
}

}//namespace r64fx