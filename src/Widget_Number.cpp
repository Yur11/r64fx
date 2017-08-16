#include "Widget_Number.hpp"
#include "WidgetFlags.hpp"
#include "InstanceCounter.hpp"
#include "FontSupply.hpp"
#include "Painter.hpp"
#include "TextEditingUtils.hpp"
#include "UndoRedoUtils.hpp"
#include "StringUtils.hpp"
#include "Float.hpp"
#include "ImageUtils.hpp"

#define m_image ((Image*)m)

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

    inline void insertText(const std::string &text) { insert_text(m_text_painter, m_undo_redo_chain, text); }

    inline void getText(std::string &text) { m_text_painter->getText(text); }

    inline void undo()
    {
        m_undo_redo_chain->undo();
        m_undo_redo_chain->removeUndone();
    }
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
    if(m_image)
        delete m_image;
}


void Widget_Number::enableTextEditing()
{
    m_flags |= R64FX_WIDGET_EDITING_TEXT;
    grabKeyboardFocus();
    startTextInput();
    setText(float2str(value()));
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


void Widget_Number::setValue(float value, bool notify)
{
    if(m_image && m_image->isGood())
    {
        fill(m_image, Color(255, 255, 255, 0));

        g.textPainter()->paintSelectionBackground(
            m_image, Color(148, 202, 239), {0, 0}
        );

        g.textPainter()->paintText(
            m_image, Color(0, 0, 0), Color(0, 0, 0), {0, 0}
        );
    }

    Value::setValue(value, notify);
}


bool Widget_Number::setText(const std::string &text)
{
    float num = str2float(text);
    if(Float32::IsNan(num))
    {
        return false;
    }

    setValue(num);
    return true;
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

    if(m_image->isGood())
        p->putImage(m_image, {0, 0});
}


void Widget_Number::resizeEvent(WidgetResizeEvent* event)
{
    if(!m_image)
        m = new Image;
    m_image->load(width(), height(), 4);
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
        if(doingTextEditing())
        {
            disableTextEditing();
        }
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
        g.insertText(text);
        string full_text;
        g.textPainter()->getText(full_text);
        if(!setText(full_text))
            g.undo();
    }
    else
    {
        return;
    }

    repaint();
}

}//namespace r64fx
