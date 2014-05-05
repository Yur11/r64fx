#include "Widget.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//DEBUG

namespace r64fx{

Widget* Widget::mouse_grabber = nullptr;
Widget* Widget::keyboard_grabber = nullptr;

void Widget::projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect)
{
    projected_rect = rect + parent_position;
    
    visible_rect = intersection_of(parent_visible_rect, projected_rect);
    
    if(visible_rect.isGood())
    {
        Widget::is_visible = true;
        
        if(hasChildren())
        {
            LinkedItemChain<Widget> non_visible_list;
            LinkedItemChain<Widget> visible_list;
            
            while(!children.isEmpty())
            {
                auto ch = children.last();
                
                if(ch == nullptr)
                {
                    abort();
                }
                ch->projectToRootAndClipVisible(projected_rect.position(), visible_rect);
                
                children.remove(ch);
#ifdef DEBUG
                assert(ch->prev() == nullptr);
                assert(ch->next() == nullptr);
#endif//DEBUG
                
                if(ch->isVisible())
                {
                    visible_list.append(ch);
                }
                else
                {
                    non_visible_list.append(ch);
                }
            }

            visible_children = { visible_list.begin(), visible_list.end() };
            
            children = non_visible_list;
            if(!visible_list.isEmpty())
                children.append(visible_list);
        }
    }
    else
    {
        Widget::is_visible = false;
    }
}


void Widget::setParent(Widget* new_parent_widget)
{    
    if(Widget::hasParent())
        Widget::removeFromParent();
    
    if(new_parent_widget)
        new_parent_widget->appendWidget(this);
}


void Widget::appendWidget(Widget* widget)
{
    widget->parent_widget = this;
    children.append(widget);
}


void Widget::insertWidget(Widget* existing_widget, Widget* widget)
{
    widget->parent_widget = this;
    children.insert(existing_widget, widget);
}


void Widget::removeFromParent()
{
    parent_widget->children.remove(this);
    parent_widget = nullptr;
}


void Widget::clear()
{
    for(auto &ch : children)
        ch.parent_widget = nullptr;
    children.clear();
}


Widget* Widget::visibleChildAt(Point<float> p)
{
    for(auto &ch : visibleChildren())
        if(ch.visibleRect().overlaps(p))
            return &ch;
    return nullptr;
}

    
void Widget::render()
{
    for(auto &ch : visibleChildren())
    {
        ch.render();
    }    
}

void Widget::mousePressEvent(MouseEvent* event)
{        
    auto child = visibleChildAt(event->position());
    if(child)
    {
        child->mousePressEvent(event);
    }
}

    
void Widget::mouseReleaseEvent(MouseEvent* event)
{
    auto child = visibleChildAt(event->position());
    if(child)
    {
        child->mouseReleaseEvent(event);
    }
}
    
    
void Widget::mouseMoveEvent(MouseEvent* event)
{
    auto child = visibleChildAt(event->position());
    if(child)
    {
        child->mouseMoveEvent(event);
    }
}


void Widget::mouseWheelEvent(MouseEvent* event)
{
}


void Widget::keyPressEvent(KeyEvent* event)
{
}
    
    
void Widget::keyReleaseEvent(KeyEvent* event)
{
}


void Widget::textInputEvent(Utf8String text)
{
    
}


void Widget::setMouseGrabber(Widget* widget)
{
    mouse_grabber = widget;
}

    
void Widget::setKeyboardGrabber(Widget* widget)
{
    keyboard_grabber = widget;
}


Widget* Widget::mouseInputGrabber()
{
    return mouse_grabber;
}
    
    
Widget* Widget::keyboardInputGrabber()
{
    return keyboard_grabber;
}


bool Widget::isMouseInputGrabber()
{
    return mouse_grabber == this;
}

    
bool Widget::isKeyboardInputGrabber()
{
    return keyboard_grabber == this;
}

}//namespace r64fx
