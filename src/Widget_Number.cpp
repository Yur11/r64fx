#include "Widget_Number.hpp"
#include "WidgetFlags.hpp"
#include "InstanceCounter.hpp"
#include "FontSupply.hpp"
#include "Painter.hpp"
#include "TextEditingUtils.hpp"
#include "UndoRedoUtils.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

class Widget_Number_Global : public InstanceCounter{
    TextPainter*    m_text_painter     = nullptr;
    UndoRedoChain*  m_undo_redo_chain  = nullptr;

    virtual void initEvent()
    {
        m_text_painter = new TextPainter;
        m_text_painter->font = get_font("mono", 14);
        m_undo_redo_chain = new UndoRedoChain;
    }

    virtual void cleanupEvent()
    {
        free_font(m_text_painter->font);
        delete m_text_painter;
        delete m_undo_redo_chain;
    }

public:
    inline Font* font() const { return m_text_painter->font; }

    inline TextPainter* textPainter() const { return m_text_painter; }

    inline UndoRedoChain* undoRedoChain() const { return m_undo_redo_chain; }
} g;

}//namespace


Widget_Number::Widget_Number(Widget* parent)
: Widget(parent)
{
    g.created();
    auto bbox = find_text_bbox("+0.000000", TextWrap::None, g.font());
    setSize({bbox.width() + 2, bbox.height() + 2});
    setMinValue(-1.0f);
}


Widget_Number::~Widget_Number()
{
    g.destroyed();
}


void Widget_Number::enableTextEditing()
{
    m_flags |= R64FX_WIDGET_EDITING_TEXT;
    grabKeyboardFocus();
    startTextInput();
    repaint();
}


void Widget_Number::disableTextEditing()
{
    m_flags &= ~R64FX_WIDGET_EDITING_TEXT;
    releaseKeyboardFocus();
    stopTextInput();
    repaint();
}


bool Widget_Number::doingTextEditing() const
{
    return m_flags & R64FX_WIDGET_EDITING_TEXT;
}


void Widget_Number::insertText(const std::string &text)
{

}


void Widget_Number::setText(const std::string &text)
{

}


void Widget_Number::addedToWindowEvent(WidgetAddedToWindowEvent* event)
{

}


void Widget_Number::removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    
}


void Widget_Number::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->strokeRect({0, 0, width(), height()}, Color(0, 0, 0), Color(255, 255, 255), 1);

    if(doingTextEditing())
    {
        p->fillRect({0, 0, width(), height()}, Color(255, 0, 0));
    }
    else
    {
        char buff[32];
        sprintf(buff, "%+f", value());
        auto img = text2image(std::string(buff), TextWrap::None, g.font());
        p->blendColors({1, 1}, Color(0, 0, 0), img);
        delete img;
    }
}


void Widget_Number::mousePressEvent(MousePressEvent* event)
{
    if(event->doubleClick())
    {
        enableTextEditing();
    }
    else
    {
        grabMouseFocus();
    }
}


void Widget_Number::mouseReleaseEvent(MouseReleaseEvent* event)
{
    releaseMouseFocus();
}


void Widget_Number::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        int dy = event->dy();
        if(event->dy() != 0)
        {
            setValue(value() + float(-dy) * valueStep(), true);
            repaint();
        }
    }
}


void Widget_Number::focusInEvent()
{
    
}


void Widget_Number::focusOutEvent()
{
    
}


void Widget_Number::keyPressEvent(KeyPressEvent* event)
{
    
}


void Widget_Number::keyReleaseEvent(KeyReleaseEvent* event)
{
    
}


void Widget_Number::textInputEvent(TextInputEvent* event)
{
    auto tp = g.textPainter();
    auto uc = g.undoRedoChain();
    auto key = event->key();
    const auto &text = event->text();
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
    else if(key == Keyboard::Key::Return)
    {
        disableTextEditing();
    }
    else if(!text.empty())
    {
        insertText(text);
    }

}

}//namespace r64fx
