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
    
 
void Widget::render_children()
{
    for(auto &child : _children)
    {
        glPushMatrix();
        glTranslatef(child->x(), child->y(), 0.0);
        child->render();
        glPopMatrix();
    }
}
    
    
void Widget::render_bounding_rect()
{
}
    
    
void Widget::render()
{
    render_children();
}


void Widget::update()
{
    for(auto ch : _children)
    {
        ch->update();
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
        auto r = ch->rect();
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


Point<float> Widget::toSceneCoords(Point<float> point)
{
    if(_parent)
    {
        return _parent->toSceneCoords(position() + point);
    }
    else
    {
        return point + position(); //We also add our position within the scene, if we are the root widget.
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


// Widget* Widget::stub()
// {
//     return &stub_widget;
// }

}//namespace r64fx
