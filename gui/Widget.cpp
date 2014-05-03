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

Widget* mouse_grabber = nullptr;
Widget* keyboard_grabber = nullptr;
Widget* hovered_widget = nullptr;


void Widget::projectToRootAndClipVisible(Rect<float> rect)
{
//     absolute_position = rect.position() + this->relativePosition();
//     if(rect.overlaps(this->absoluteRect()))
//     {
//         is_visible = true;
//         
//         if(hasChildren())
//         {
//             for(auto w : allChildren())
//                 w->projectToRootAndClipVisible(absoluteRect());
//             
//             sort(allChildren().begin(), allChildren().end(), [](Widget* a, Widget* b){
//                 return (!a->isVisible()) && b->isVisible();
//             });
//             
//             auto b = allChildren().begin();
//             auto e = allChildren().end();
//             
//             while(!(*b)->isVisible())
//             {
// #ifdef DEBUG
//                 assert(b != allChildren().end());
// #endif//DEBUG
//                 b++;
//             }
//             
//             visible_children = { b, e };
//         }
//     }
//     else
//     {
//         is_visible = false;
//     }
}


void Widget::setParent(Widget* newparent_widget)
{
//     if(parent_widget)
//     {
//         /* Remove me from the old parent. */
//         for(auto it = parent_widget->allChildren().begin(); it != parent_widget->allChildren().end(); it++)
//         {
//             if(*it == this)
//             {
//                 parent_widget->children.erase(it);
//                 break;
//             }
//         }
//     }
//     
//     parent_widget = newparent_widget;
//     
//     if(parent_widget)
//     {
//         parent_widget->children.push_back(this);
//     }
}


void Widget::appendWidget(Widget* widget)
{
//     widget->parent_widget = this;
//     children.push_back(widget);
}


void Widget::insertWidget(Widget* existing_widget, Widget* widget)
{
//     widget->parent_widget = this;
//     children.insert(children.begin() + index, widget);
}


void Widget::clear()
{
//     while(!children.empty())
//         removeChild(children.size() - 1);
    
    for(auto &ch : children)
        ch.parent_widget = nullptr;
    children.clear();
}


void Widget::update()
{
    if(parent_widget == nullptr)
        absolute_position = relative_position;
    
    projectToRootAndClipVisible(absoluteRect());
}
    
    
void Widget::render()
{
    for(auto ch : allChildren())
    {
        ch.render();
    }
}

void Widget::mousePressEvent(MouseEvent* event)
{        
    auto child = find_widget_absolute(event->position(), visibleChildren());
    if(child)
    {
        child->mousePressEvent(event);
    }
}

    
void Widget::mouseReleaseEvent(MouseEvent* event)
{
    auto child = find_widget_absolute(event->position(), visibleChildren()); 
    if(child)
    {
        child->mouseReleaseEvent(event);
    }
}
    
    
void Widget::mouseMoveEvent(MouseEvent* event)
{
    auto child = find_widget_absolute(event->position(), visibleChildren()); 
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


Point<float> Widget::toParentCoords(Point<float> point)
{
    return relativePosition() + point;
}


// Point<float> Widget::toSuperCoordinates(Point<float> point, Widget* super)
// {
// #ifdef DEBUG
//     assert(super != nullptr);
//     
//     if(!parent_widget)
//     {
//         cerr << "Widget::toSuperCoordinates(): The given super widget does not belong to this widget tree!\n";
//         abort();
//     }
// #endif//DEBUG
//     
//     if(parent_widget == super)
//     {
//         return point + this->relativePosition();
//     }
//     else
//     {
//         return parent_widget->toSuperCoordinates(point, super) + this->relativePosition();
//     }
// }

    
Point<float> Widget::toRootCoords(Point<float> point)
{
    if(parent_widget)
    {
        return parent_widget->toRootCoords(relativePosition() + point);
    }
    else
    {
        return point;
    }
}


Widget* Widget::root()
{
    if(!parent_widget)
    {
        return this;
    }
    else
    {
        return parent_widget->root();
    }
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


bool Widget::isMouseGrabber()
{
    return mouse_grabber == this;
}

    
bool Widget::isKeyboardGrabber()
{
    return keyboard_grabber == this;
}


Widget* find_widget_relative(Point<float> p, WidgetIteratorPair range)
{
    for(auto &w : range)
        if(w.relativeRect().overlaps(p))
            return &w;
    return nullptr;
}


Widget* find_widget_absolute(Point<float> p, WidgetIteratorPair range)
{
    for(auto &w : range)
    {
        if(w.absoluteRect().overlaps(p))
            return &w;
    }
    return nullptr;
}




}//namespace r64fx
