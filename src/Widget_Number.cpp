#include "Widget_Number.hpp"
#include "WidgetFlags.hpp"
#include "InstanceCounter.hpp"
#include "FontSupply.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "UndoRedoChain.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

class Widget_Number_Global : public InstanceCounter{
    TextPainter* m_text_painter = nullptr;

    virtual void initEvent()
    {
        m_text_painter = new TextPainter;
        m_text_painter->font = get_font("mono", 14);
    }

    virtual void cleanupEvent()
    {
        free_font(m_text_painter->font);
        delete m_text_painter;
    }

public:
    inline Font* font() const { return m_text_painter->font; }

    inline TextPainter* textPainter() const { return m_text_painter; }
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
}


void Widget_Number::disableTextEditing()
{
    m_flags &= ~R64FX_WIDGET_EDITING_TEXT;
}


bool Widget_Number::doingTextEditing() const
{
    return m_flags & R64FX_WIDGET_EDITING_TEXT;
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

    char buff[32];
    sprintf(buff, "%+f", value());
    auto img = text2image(std::string(buff), TextWrap::None, g.font());
    p->blendColors({1, 1}, Color(0, 0, 0), img);
    delete img;
}


void Widget_Number::mousePressEvent(MousePressEvent* event)
{
    if(event->doubleClick())
    {
        grabKeyboardFocus();
        startTextInput();
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
    auto key = event->key();
    const auto &text = event->text();
    bool touched_selection = false;

    if(event->key() == Keyboard::Key::Escape)
    {
        releaseKeyboardFocus();
    }
    else if(Keyboard::CtrlDown() && Keyboard::ShiftDown() && event->key() == Keyboard::Key::Z)
    {
//         uc->redo();
    }
    else if(Keyboard::CtrlDown() && event->key() == Keyboard::Key::Z)
    {
//         uc->undo();
    }
    else if(Keyboard::CtrlDown() && (event->key() == Keyboard::Key::Y))
    {
//         uc->redo();
    }
    else if(key == Keyboard::Key::Up)
    {
//         if(Keyboard::ShiftDown())
//         {
// //             tp->selectUp();
// //             touched_selection = true;
//         }
//         else
//         {
//             if(tp->hasSelection())
//             {
//                 tp->clearSelection();
//             }
//             tp->moveCursorUp();
//         }
    }
    else if(key == Keyboard::Key::Down)
    {
//         if(Keyboard::ShiftDown())
//         {
//             tp->selectDown();
//             touched_selection = true;
//         }
//         else
//         {
//             if(tp->hasSelection())
//             {
//                 tp->clearSelection();
//             }
//             tp->moveCursorDown();
//         }
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
//         if(tp->hasSelection())
//         {
//             g_clipboard_text = tp->selectionText();
//             anounceClipboardData("text/plain", ClipboardMode::Clipboard);
//         }
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::X)
    {
/*        if(tp->hasSelection())
        {
            g_clipboard_text = tp->selectionText();
            anounceClipboardData("text/plain", ClipboardMode::Clipboard);
            deleteAtCursorPosition(false);
        }*/
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::V)
    {
//         requestClipboardMetadata(ClipboardMode::Clipboard);
    }
    else if(key == Keyboard::Key::Delete)
    {
//         deleteAtCursorPosition(false);
    }
    else if(key == Keyboard::Key::Backspace)
    {
//         deleteAtCursorPosition(true);
    }
    else if(key == Keyboard::Key::Return)
    {
//         insertText("\n");
    }
    else if(!text.empty())
    {
//         insertText(text);
    }
}

}//namespace r64fx
