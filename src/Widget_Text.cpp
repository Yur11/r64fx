#include "Widget_Text.hpp"
#include "WidgetFlags.hpp"
#include "Font.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "ClipboardEvent.hpp"
#include "Mouse.hpp"
#include "Program.hpp"
#include "TextPainter.hpp"
#include "UndoRedoChain.hpp"
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


namespace{

class CursorsMixin{
    int m_cursor_position_before;
    int m_selection_start_before;
    int m_selection_end_before;

    int m_cursor_position_after;
    int m_selection_start_after;
    int m_selection_end_after;

public:
    void saveCursorsBefore(TextPainter* tp)
    {
        m_cursor_position_before = tp->cursorPositionToGlyphIndex(
            tp->cursorPosition()
        );

        m_selection_start_before = tp->cursorPositionToGlyphIndex(
            tp->selectionStart()
        );

        m_selection_end_before   = tp->cursorPositionToGlyphIndex(
            tp->selectionEnd()
        );
    }

    void saveCursorsAfter(TextPainter* tp)
    {
        m_cursor_position_after = tp->cursorPositionToGlyphIndex(
            tp->cursorPosition()
        );

        m_selection_start_after = tp->cursorPositionToGlyphIndex(
            tp->selectionStart()
        );

        m_selection_end_after   = tp->cursorPositionToGlyphIndex(
            tp->selectionEnd()
        );
    }

    void restoreCursorsBefore(TextPainter* tp)
    {
        tp->setCursorPosition(tp->glyphIndexToCursorPosition(
            m_cursor_position_before
        ));

        tp->setSelectionStart(tp->glyphIndexToCursorPosition(
            m_selection_start_before
        ));

        tp->setSelectionEnd(tp->glyphIndexToCursorPosition(
            m_selection_end_before
        ));
    }

    void restoreCursorsAfter(TextPainter* tp)
    {
        tp->setCursorPosition(tp->glyphIndexToCursorPosition(
            m_cursor_position_after
        ));

        tp->setSelectionStart(tp->glyphIndexToCursorPosition(
            m_selection_start_after
        ));

        tp->setSelectionEnd(tp->glyphIndexToCursorPosition(
            m_selection_end_after
        ));
    }

    inline int cursorPositionBefore() const
    {
        return m_cursor_position_before;
    }
};


class TextAddedMixin{
    GlyphString m_added_glyphs;

public:
    inline void saveAddedGlyphs(const GlyphString &glyphs)
    {
        m_added_glyphs = glyphs;
    }

    inline const GlyphString &addedGlyphs() const
    {
        return m_added_glyphs;
    }
};


class TextRemovedMixin{
    GlyphString m_removed_glyphs;

public:
    inline void saveRemovedGlyphs(const GlyphString &glyphs)
    {
        m_removed_glyphs = glyphs;
    }

    inline const GlyphString &removedGlyphs() const
    {
        return m_removed_glyphs;
    }
};


class UndoRedoItem_TextAdded
: public UndoRedoItem
, public CursorsMixin
, public TextAddedMixin
{
public:
    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;

        tp->setSelectionStart(tp->glyphIndexToCursorPosition(
            cursorPositionBefore()
        ));

        tp->setSelectionEnd(tp->glyphIndexToCursorPosition(
            cursorPositionBefore() + addedGlyphs().size()
        ));

        tp->deleteSelection();

        restoreCursorsBefore(tp);
        tp->updateSelection();
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;
        restoreCursorsBefore(tp);
        tp->insertText(addedGlyphs());
        restoreCursorsAfter(tp);
        tp->updateSelection();
    }
};


class UndoRedoItem_TextDeleted
: public UndoRedoItem
, public CursorsMixin
, public TextRemovedMixin{
    bool m_removed_before_cursor; //Delete or Backspace

public:
    UndoRedoItem_TextDeleted(bool removed_before_cursor)
    : m_removed_before_cursor(removed_before_cursor)
    {}

    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;
        restoreCursorsAfter(tp);
        tp->insertText(removedGlyphs());
        restoreCursorsBefore(tp);
        tp->updateSelection();
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;
        restoreCursorsBefore(tp);
        if(m_removed_before_cursor)
        {
            tp->deleteBeforeCursor();
        }
        else
        {
            tp->deleteAfterCursor();
        }
        restoreCursorsAfter(tp);
        tp->updateSelection();
    }
};


class UndoRedoItem_TextReplaced
: public UndoRedoItem
, public CursorsMixin
, public TextRemovedMixin
, public TextAddedMixin{
public:
    virtual void undo(void* data)
    {
        auto tp = (TextPainter*) data;

        tp->setSelectionStart(tp->glyphIndexToCursorPosition(
            cursorPositionBefore()
        ));

        tp->setSelectionEnd(tp->glyphIndexToCursorPosition(
            cursorPositionBefore() + addedGlyphs().size()
        ));

        tp->deleteAfterCursor();
        tp->insertText(removedGlyphs());

        restoreCursorsBefore(tp);
        tp->updateSelection();
    }

    virtual void redo(void* data)
    {
        auto tp = (TextPainter*) data;
        restoreCursorsBefore(tp);
        tp->insertText(addedGlyphs());
        restoreCursorsAfter(tp);
        tp->updateSelection();
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

    if(tp->hasSelection())
    {
        auto item = new UndoRedoItem_TextReplaced;
        item->saveCursorsBefore(tp);
        GlyphString removed_glyphs, added_glyphs;
        tp->insertText(text, &removed_glyphs, &added_glyphs);
        item->saveRemovedGlyphs(removed_glyphs);
        item->saveAddedGlyphs(added_glyphs);
        item->saveCursorsAfter(tp);
        uc->addItem(item);
    }
    else
    {
        auto item = new UndoRedoItem_TextAdded;
        item->saveCursorsBefore(tp);
        GlyphString added_glyphs;
        tp->insertText(text, nullptr, &added_glyphs);
        item->saveAddedGlyphs(added_glyphs);
        item->saveCursorsAfter(tp);
        uc->addItem(item);
    }
}


void Widget_Text::deleteAtCursorPosition(bool backspace)
{
    auto tp   = m_text_painter;
    auto uc   = m_undo_redo_chain;
    auto item = new UndoRedoItem_TextDeleted(true);
    item->saveCursorsBefore(tp);
    GlyphString glyphs;
    if(backspace)
        tp->deleteBeforeCursor(&glyphs);
    else
        tp->deleteAfterCursor(&glyphs);
    item->saveRemovedGlyphs(glyphs);
    item->saveCursorsAfter(tp);
    uc->addItem(item);
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
    if(isMouseGrabber())
    {
        unsigned char color[4] = {255, 127, 127, 0};
        fill(m_image, color);
    }
    else
    {
        unsigned char color[4] = {255, 255, 255, 0};
        fill(m_image, color);
    }

    m_text_painter->paintSelectionBackground(
        m_image, {148, 202, 239}, {paddingLeft(), paddingTop()}
    );

    m_text_painter->paintText(
        m_image, {0, 0, 0}, {0, 0, 0}, {paddingLeft(), paddingTop()}
    );

//     draw_rect(
//         m_image, {0, 0, 0}, {0, 0, width(), height()}
//     );

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
    m_text_painter->setReflowWidth(event->width() - paddingLeft() - paddingRight());
    m_text_painter->reflow();
    m_text_painter->reallign();
}


void Widget_Text::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
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
    update();
}


void Widget_Text::mouseReleaseEvent(MouseReleaseEvent* event)
{
    Widget::mouseReleaseEvent(event);
    if(m_text_painter->hasSelection())
    {
        g_selection_text = m_text_painter->selectionText();
        anounceClipboardData("text/plain", ClipboardMode::Selection);
    }
    update();
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
    bool touched_selection = false;

    if(event->key() == Keyboard::Key::Escape)
    {
        removeFocus();
    }
    else if(Keyboard::CtrlDown() && Keyboard::ShiftDown() && event->key() == Keyboard::Key::Z)
    {
        uc->redo();
    }
    else if(Keyboard::CtrlDown() && event->key() == Keyboard::Key::Z)
    {
        uc->undo();
    }
    else if(Keyboard::CtrlDown() && (event->key() == Keyboard::Key::Y))
    {
        uc->redo();
    }
    else if(key == Keyboard::Key::Up)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectUp();
            touched_selection = true;
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
            touched_selection = true;
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
            touched_selection = true;
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
            touched_selection = true;
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
            touched_selection = true;
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
            touched_selection = true;
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
        touched_selection = true;
    }
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
    else if(key == Keyboard::Key::Delete)
    {
        deleteAtCursorPosition(false);
    }
    else if(key == Keyboard::Key::Backspace)
    {
        deleteAtCursorPosition(true);
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

    update();
}


void Widget_Text::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{
    cout << event->type().name() << "\n";
    if(event->mode() != ClipboardMode::Bad && event->data() != nullptr && event->size() > 0)
    {
        string text((const char*)event->data(), event->size());
        insertText(text);
        update();
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