#include "Widget_Number.hpp"
#include "WidgetFlags.hpp"
#include "InstanceCounter.hpp"
#include "FontSupply.hpp"
#include "Painter.hpp"
#include "TextEditingUtils.hpp"
#include "UndoRedoUtils.hpp"
#include "StringUtils.hpp"
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
} g;


string float2str(float f)
{
    if(f == 0.0f)
        return "0";

    char buff[64];
    sprintf(buff, "%.3f", f);  
    string str((const char*)buff);

    return str;
}


float str2float(const string &str)
{
    float f = 0.0f;
    float c = 0.1f;
    bool whole_part = true;
    for(auto ch : str)
    {
        if(whole_part && ch == '.')
        {
            whole_part = false;
        }
        else if(ch < '0' || ch > '9')
        {
            return nanf("");
        }
        else
        {
            float a = float(ch - '0');
            if(whole_part)
            {
                f = f * 10.0f + a;
            }
            else
            {
                f = f + a * c;
                c *= 0.1f;
            }
        }
    }
    return f;
}

}//namespace


Widget_Number::Widget_Number(Widget* parent)
: Widget(parent)
{
    g.created();
    auto bbox = find_text_bbox("1234567", TextWrap::None, g.font());
    setSize({bbox.width() + 2, bbox.height() + 2});
    setMinValue(-1.0f);
    setMaxValue(+1.0f);
    setValueStep(0.005f);
    Value::setValue(0.0f);
}


Widget_Number::~Widget_Number()
{
    g.destroyed();
    if(m_image)
        delete m_image;
}


void Widget_Number::beginTextEditing()
{
    m_flags |= R64FX_WIDGET_EDITING_TEXT;
    grabKeyboardFocus();
    startTextInput();

    auto tp = g.textPainter();
    tp->clear();
    tp->insertText(float2str(value()));
    tp->selectAll();

    renderImage();
    repaint();
}


void Widget_Number::endTextEditing(bool commit)
{
    if(!doingTextEditing())
        return;

    auto tp = g.textPainter();
    auto uc = g.undoRedoChain();

    m_flags &= ~R64FX_WIDGET_EDITING_TEXT;
    releaseKeyboardFocus();
    stopTextInput();

    if(commit)
    {
        string str;
        tp->getText(str);
        if(str.empty())
            setValue(0.0f, true);
        else
            setValue(str2float(str), true);
    }
    else
    {
        setValue(Value::value(), true);
    }

    repaint();

    tp->clear();
    uc->clear();
}


bool Widget_Number::doingTextEditing() const
{
    return m_flags & R64FX_WIDGET_EDITING_TEXT;
}


void Widget_Number::setValue(float value, bool notify)
{
    endTextEditing(false);

    auto tp = g.textPainter();

    Value::setValue(value, notify);
    tp->clear();
    tp->insertText(float2str(Value::value()));
    renderImage();
    tp->clear();
}


void Widget_Number::setValue(const std::string &text, bool notify)
{
    float value = str2float(text);
    if(!isnan(value))
    {
        setValue(value);
    }
}


void Widget_Number::renderImage()
{
    if(!m_image || !m_image->isGood())
        return;

    auto tp = g.textPainter();

    if(doingTextEditing())
    {
        fill(m_image, Color(255, 255, 255, 0));

        tp->paintSelectionBackground(
            m_image, Color(148, 202, 239), {0, 0}
        );

        tp->paintText(
            m_image, Color(0, 0, 0), Color(0, 0, 0), {0, 0}
        );
    }
    else
    {
        fill(m_image, Color(127, 127, 127, 0));

        tp->paintText(
            m_image, Color(0, 0, 0), Color(0, 0, 0), {0, 0}
        );
    }
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

    if(m_image->isGood())
        p->putImage(m_image, {0, 0});

    if(doingTextEditing())
    {
        auto tp = g.textPainter();
        auto cursor_pos = tp->findCursorCoords(tp->cursorPosition());
        p->fillRect({cursor_pos.x(), cursor_pos.y(), 2, tp->font->height()}, Color(0, 0, 0));
    }
}


void Widget_Number::resizeEvent(WidgetResizeEvent* event)
{
    if(!m_image)
        m = new Image;
    m_image->load(width(), height(), 4);
    setValue(Value::value());
    repaint();
}


void Widget_Number::mousePressEvent(MousePressEvent* event)
{
    if(doingTextEditing())
    {
        if(event->button() == MouseButton::Left())
        {
            auto tp = g.textPainter();
            auto tcp = tp->findCursorPosition(
                event->position()
            );
            tp->setCursorPosition(tcp);
            if(event->doubleClick())
            {
                tp->selectAll();
            }
            else
            {
                tp->setSelectionStart(tcp);
                tp->setSelectionEnd(tcp);
                tp->updateSelection();
            }
        }

        if(event->button() == MouseButton::Left() && !Rect<int>(0, 0, width(), height()).overlaps(event->position()))
        {
            endTextEditing(false);
            releaseMouseFocus();
        }
    }
    else
    {
        if(event->button() == MouseButton::Left())
        {
            if(event->doubleClick())
            {
                beginTextEditing();
            }
            grabMouseFocus();
        }
    }
    repaint();
}


void Widget_Number::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(!doingTextEditing())
    {
        releaseMouseFocus();
    }
}


void Widget_Number::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(doingTextEditing())
        {
            auto tp = g.textPainter();
            auto tcp = tp->findCursorPosition(
                event->position()
            );
            tp->setCursorPosition(tcp);
            tp->setSelectionEnd(tcp);
            tp->updateSelection();
            renderImage();
            repaint();
        }
        else
        {
            int dy = event->dy();
            if(event->dy() != 0)
            {
                setValue(value() + float(-dy) * valueStep(), true);
                repaint();
            }
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

    if(event->key() == Keyboard::Key::Escape || key == Keyboard::Key::Return)
    {
        if(doingTextEditing())
        {
            endTextEditing(key == Keyboard::Key::Return);
        }
    }
    else if(
        cursor_hori(tp, key, &touched_selection) ||
        cursor_vert(tp, key, &touched_selection) ||
        select_all(tp, key, &touched_selection)
    )
    {
        /* Selection May Have Changed */
        renderImage();
        repaint();
    }
    else
    {
        if( undo_redo(uc, key) || delete_text(tp, key, uc, &touched_selection))
        {}
        else if(!text.empty())
        {
            insert_text(tp, uc, text);
        }

        /* Text Has Changed */
        string str;
        tp->getText(str);
        cout << str << "\n";
        float value = str2float(str);
        if(isnan(value))
        {
            uc->undo();
            uc->removeUndone();
        }
        else
        {
            renderImage();
            repaint();
        }
    }
}

}//namespace r64fx
