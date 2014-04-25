#include "Widget.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//DEBUG

namespace r64fx{

Widget* mouse_grabber = nullptr;
Widget* keyboard_grabber = nullptr;
Widget* hovered_widget = nullptr;


Widget stub_widget;

void Widget::setParent(Widget* new_parent)
{
    if(_parent)
    {
        /* Remove me from the old parent. */
        for(auto it = _parent->_children.begin(); it != _parent->_children.end(); it++)
        {
            if(*it == this)
            {
                _parent->_children.erase(it);
                break;
            }
        }
    }
    
    _parent = new_parent;
    
    if(_parent)
    {
        _parent->_children.push_back(this);
    }
}


void Widget::appendWidget(Widget* widget)
{
    widget->_parent = this;
    _children.push_back(widget);
}


void Widget::insertWidget(Widget* widget, int index)
{
    widget->_parent = this;
    _children.insert(_children.begin() + index, widget);
}


void Widget::clear()
{
    while(!_children.empty())
        _children[_children.size() - 1]->setParent(nullptr);
}
    
    
void Widget::render()
{
    for(auto ch : /*visible*/_children)
    {
        ch->render();
    }
}


void Widget::clip(Rect<float> rect)
{
    visible_children.clear();
    for(auto ch : _children)
    {
        if(rect.overlaps(ch->projectedRect()))
        {
            visible_children.push_back(ch);
            ch->clip(ch->projectedRect());
        }
    }
}


void Widget::project(Point<float> offset)
{
    projected_position = offset;
    for(auto ch : _children)
    {
        ch->project(projected_position + ch->position());
    }
}


void Widget::updateVisuals()
{
    for(auto ch : visible_children)
    {
        ch->updateVisuals();
    }
}


void Widget::mousePressEvent(MouseEvent* event)
{
//     event->widget = this;
    
    auto child = childAt(event->position()); 
    if(child)
    {
        *event -= child->position();
        child->mousePressEvent(event);
    }
}

    
void Widget::mouseReleaseEvent(MouseEvent* event)
{
    auto child = childAt(event->position()); 
    if(child)
    {
        *event -= child->position();
        child->mouseReleaseEvent(event);
    }
}
    
    
void Widget::mouseMoveEvent(MouseEvent* event)
{
    auto child = childAt(event->position()); 
    if(child)
    {
        *event -= child->position();
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


Widget* Widget::childAt(float x, float y)
{
    for(int i=0; i<(int)childrenCount(); i++)
    {
        auto ch = child(i);
        auto r = ch->boundingRect();
        if(r.overlaps(x, y))
        {
            return ch;
        }
    }
    
    return nullptr;
}

Point<float> Widget::toParentCoords(Point<float> point)
{
    return position() + point;
}


Point<float> Widget::toSuperCoordinates(Point<float> point, Widget* super)
{
#ifdef DEBUG
    assert(super != nullptr);
    
    if(!_parent)
    {
        cerr << "Widget::toSuperCoordinates(): The given super widget does not belong to this widget tree!\n";
        abort();
    }
#endif//DEBUG
    
    if(_parent == super)
    {
        return point + this->position();
    }
    else
    {
        return _parent->toSuperCoordinates(point, super) + this->position();
    }
}

    
Point<float> Widget::toRootCoords(Point<float> point)
{
    if(_parent)
    {
        return _parent->toRootCoords(position() + point);
    }
    else
    {
        return point;
    }
}


Widget* Widget::root()
{
    if(!_parent)
    {
        return this;
    }
    else
    {
        return _parent->root();
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


Widget* Widget::findLeafAt(float x, float y)
{
    Widget* child = childAt(x, y);
    if(child)
    {
        return child->findLeafAt(x - child->x(), y - child->y());
    }
    else
    {
        return this;
    }
}


}//namespace r64fx
