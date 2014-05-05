#include "TextEdit.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Keyboard.h"

#include <iostream>

using namespace std;

namespace r64fx{
    
void TextEdit::render()
{    
    /* Draw background. */
//     glColor(this->backgroundColor());
//     glBegin(GL_TRIANGLE_FAN);
//         glVertex2f(0.0, 0.0);
//         glVertex2f(Widget::width() - 1, 0.0);
//         glVertex2f(Widget::width() - 1, Widget::height() -1);
//         glVertex2f(0.0,                 Widget::height() -1);
//     glEnd();
    
    /* Draw text. */
    TextLine::render();
    
    if(text_selection.drag_occured())
    {
        /* Draw selection. */
        float left = text_selection.left() + 1;
        float top = Padding::paddingTop() + 1;
        float right = text_selection.right() - 1;
        float bottom = Widget::height() - Padding::paddingBottom() - 1;
    }
    else
    {
        if(Widget::isKeyboardInputGrabber())
        {
            /* Draw cursor. */
            float cursor_x = Padding::paddingLeft();
            int n = cursor_position;

            cursor_x += font()->lineAdvance(text[n]);
        }
    }
}


void TextEdit::mousePressEvent(MouseEvent* event)
{
    if(!Widget::isKeyboardInputGrabber())
    {
        prev_keyboard_grabber = Widget::keyboardInputGrabber();
        Widget::grabKeyboardInput();
        
        Keyboard::beginTextInput();
    }
        
    if(event->buttons() & Mouse::Button::Left)
    {
        cursor_position = charAt(event->x()) - 1;
        if(cursor_position < 0) cursor_position = 0;
        std::cout << cursor_position << " clicked: " << this->text[cursor_position] << "\n";
        
        text_selection.clear();
    }
}
    
    
void TextEdit::mouseReleaseEvent(MouseEvent* event)
{
    cout << "edit mouse release\n";
    if(event->buttons() & Mouse::Button::Left)
    {
        text_selection.drag_end();
        if(text_selection.left() == text_selection.right()) text_selection.clear();
    }
    if(isMouseInputGrabber()) Widget::setMouseGrabber(prev_mouse_grabber);
}


void TextEdit::mouseMoveEvent(MouseEvent* event)
{
    if(event->buttons() & Mouse::Button::Left)
    {
        if(text_selection.drag_is_actve())
        {
            text_selection.drag(charAt(event->x()), event->x());
            std::cout << "drag: " << text_selection.left_index() << ", " << text_selection.right_index() << "\n";
        }
        else
        {
            auto char_index = charAt(event->x());
            float x = Padding::paddingLeft();
            if(char_index > 0)
            {
                x += font()->lineAdvance(text[char_index]);
            }
            text_selection.drag_start(char_index, x);
            prev_mouse_grabber = Widget::mouseInputGrabber();
            Widget::grabMouseInput();
        }
    }
}
    
    
void TextEdit::keyPressEvent(KeyEvent* event)
{
    if(event->key() == Keyboard::Key::Escape)
    {
        Widget::setKeyboardGrabber(prev_keyboard_grabber);
        Keyboard::endTextInput();
    }
    else if(event->key() == Keyboard::Key::Return)
    {
        Widget::setKeyboardGrabber(prev_keyboard_grabber);
        Keyboard::endTextInput();
    }
    else if(event->key() == Keyboard::Key::Right)
    {
        if(text_selection.drag_occured())
        {
            
        }
        else
        {
            move_cursor_forward();
        }
    }
    else if(event->key() == Keyboard::Key::Left)
    {
        if(text_selection.drag_occured())
        {
            
        }
        else
        {
            move_cursor_back();
        }
    }
    else if(event->key() == Keyboard::Key::Home)
    {
        home_cursor();
    }
    else if(event->key() == Keyboard::Key::End)
    {
        end_cursor();
    }
    else if(event->key() == Keyboard::Key::Delete)
    {
        erase_forward();
    }
    else if(event->key() == Keyboard::Key::Backspace)
    {
        erase_backwards();
    }
}


void TextEdit::textInputEvent(Utf8String str)
{
    cout << "text input: " << str.stdstr << "\n";
    for(int i=0; i<(int)str.size(); i++)
    {
        auto ch = str[i];
        cout << "cursor_position: \n" << cursor_position << "\n";
        text.insert(cursor_position, 1, ch);
        move_cursor_forward();
    }
}


int TextEdit::charAt(float x)
{
    x -= Padding::paddingLeft();
    int i=0;
    while(i<(int)text.size())
    {
        int adv = font()->lineAdvance(text[i]);
        if(adv > x) return i;
        i++;
    }
    
    return i;
}
    
}//namespace r64fx
