#include "TextEditingUtils.hpp"


namespace r64fx{

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


class ParentTextPainterMixin{
protected:
    TextPainter* m_text_painter = nullptr;

    ParentTextPainterMixin(TextPainter* text_painter)
    : m_text_painter(text_painter)
    {}
};


class UndoRedoItem_TextAdded
: public UndoRedoItem
, public CursorsMixin
, public TextAddedMixin
, public ParentTextPainterMixin
{
public:
    UndoRedoItem_TextAdded(TextPainter* text_painter)
    : ParentTextPainterMixin(text_painter)
    {}

    virtual void undo() override final
    {
        auto tp = m_text_painter;

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

    virtual void redo() override final
    {
        auto tp = m_text_painter;
        restoreCursorsBefore(tp);
        tp->insertText(addedGlyphs());
        restoreCursorsAfter(tp);
        tp->updateSelection();
    }
};


class UndoRedoItem_TextDeleted
: public UndoRedoItem
, public CursorsMixin
, public TextRemovedMixin
, public ParentTextPainterMixin
{
    bool m_removed_before_cursor; //Delete or Backspace

public:
    UndoRedoItem_TextDeleted(TextPainter* text_painter, bool removed_before_cursor)
    : ParentTextPainterMixin(text_painter)
    , m_removed_before_cursor(removed_before_cursor)
    {}

    virtual void undo() override final
    {
        auto tp = m_text_painter;
        restoreCursorsAfter(tp);
        tp->insertText(removedGlyphs());
        restoreCursorsBefore(tp);
        tp->updateSelection();
    }

    virtual void redo() override final
    {
        auto tp = m_text_painter;
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
, public TextAddedMixin
, public ParentTextPainterMixin
{
public:
    UndoRedoItem_TextReplaced(TextPainter* text_painter)
    : ParentTextPainterMixin(text_painter)
    {}

    virtual void undo() override final
    {
        auto tp = m_text_painter;

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

    virtual void redo() override final
    {
        auto tp = m_text_painter;
        restoreCursorsBefore(tp);
        tp->insertText(addedGlyphs());
        restoreCursorsAfter(tp);
        tp->updateSelection();
    }
};

}//namespace


bool cursor_hori(TextPainter* tp, int key, bool* touched_selection)
{
    if(key == Keyboard::Key::Left)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectLeft();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorLeft();
        }
        return true;
    }
    else if(key == Keyboard::Key::Right)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectRight();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorRight();
        }
        return true;
    }
    else if(key == Keyboard::Key::Home)
    {
        if(Keyboard::ShiftDown())
        {
            tp->homeSelection();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->homeCursor();
        }
        return true;
    }
    else if(key == Keyboard::Key::End)
    {
        if(Keyboard::ShiftDown())
        {
            tp->endSelection();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->endCursor();
        }
        return true;
    }
    return false;
}


bool cursor_vert(TextPainter* tp, int key, bool* touched_selection)
{
    if(key == Keyboard::Key::Up)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectUp();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorUp();
        }
        return true;
    }
    else if(key == Keyboard::Key::Down)
    {
        if(Keyboard::ShiftDown())
        {
            tp->selectDown();
            *touched_selection = true;
        }
        else
        {
            if(tp->hasSelection())
            {
                tp->clearSelection();
            }
            tp->moveCursorDown();
        }
        return true;
    }
    return false;
}


bool select_all(TextPainter* tp, int key, bool* touched_selection)
{
    if(Keyboard::CtrlDown() && key == Keyboard::Key::A)
    {
        tp->selectAll();
        *touched_selection = true;
        return true;
    }
    return false;
}


void delete_text_at_cursor(TextPainter* tp, UndoRedoChain* uc, bool backspace)
{
    auto item = new UndoRedoItem_TextDeleted(tp, true);
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


bool delete_text(TextPainter* tp, int key, UndoRedoChain* uc, bool* touched_selection)
{
    if(key == Keyboard::Key::Delete)
    {
        delete_text_at_cursor(tp, uc, false);
        return true;
    }
    else if(key == Keyboard::Key::Backspace)
    {
        delete_text_at_cursor(tp, uc, true);
        return true;
    }
    return false;
}


bool insert_text(TextPainter* tp, UndoRedoChain* uc, const std::string &text)
{
    if(tp->hasSelection())
    {
        auto item = new UndoRedoItem_TextReplaced(tp);
        item->saveCursorsBefore(tp);
        GlyphString removed_glyphs, added_glyphs;
        tp->insertText(text, &removed_glyphs, &added_glyphs);
        item->saveRemovedGlyphs(removed_glyphs);
        item->saveAddedGlyphs(added_glyphs);
        item->saveCursorsAfter(tp);
        uc->addItem(item);
        return true;
    }
    else
    {
        auto item = new UndoRedoItem_TextAdded(tp);
        item->saveCursorsBefore(tp);
        GlyphString added_glyphs;
        tp->insertText(text, nullptr, &added_glyphs);
        item->saveAddedGlyphs(added_glyphs);
        item->saveCursorsAfter(tp);
        uc->addItem(item);
        return true;
    }
    return false;
}

}//namespace r64fx
