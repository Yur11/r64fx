#include "Widget.hpp"
#include "Window.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "ReconfigureEvent.hpp"
#include "Image.hpp"
#include "Painter.hpp"

#include <map>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using std::cerr;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

Widget::Widget(Widget* parent)
{
    setParent(parent);
}


Widget::~Widget()
{
    
}

    
void Widget::setParent(Widget* parent)
{
    if(isWindow())
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::setParent()\nTrying to set parent on a window!\n";
#endif//R64FX_DEBUG
        return;
    }

    if(!parent)
    {
        if(m_parent.widget)
        {
            m_parent.widget->m_children.remove(this);
        }
    }
    else
    {
        parent->m_children.append(this);
    }
    m_parent.widget = parent;
}


Widget* Widget::parent() const
{
    return (Widget*)(isWindow() ? nullptr : m_parent.widget);
}

Window* Widget::parentWindow() const
{
    return (Window*)(isWindow() ? m_parent.window : nullptr);
}


void Widget::add(Widget* child)
{
    if(!child)
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::add(nullptr)\n";
#endif//R64FX_DEBUG
        return;
    }
    child->setParent(this);
}


Widget* Widget::root()
{
    if(isWindow())
    {
        return this;
    }
    else if(m_parent.widget)
    {
        return m_parent.widget->root();
    }
    else
    {
        return this;
    }
}


void Widget::setPosition(Point<int> pos)
{
    m_rect.setPosition(pos);
}


Point<int> Widget::position() const
{
    return m_rect.position();
}


void Widget::setSize(Size<int> size)
{
    m_rect.setSize(size);
    if(isWindow())
    {
        m_parent.window->resize(size.w, size.h);
    }
}


Size<int> Widget::size() const
{
    return m_rect.size();
}


int Widget::width() const
{
    return m_rect.width();
}


int Widget::height() const
{
    return m_rect.height();
}


Rect<int> Widget::rect() const
{
    return m_rect;
}


void Widget::show()
{
    if(!isWindow())
    {
        m_parent.window = Window::newInstance(
            width(), height(), "", Window::Type::GL
        );
        m_parent.window->setWidget(this);
        m_flags |= R64FX_WIDGET_IS_WINDOW;
    }
    m_parent.window->show();
    m_parent.window->resize(width(), height());
}


void Widget::hide()
{
    if(isWindow())
    {
        m_parent.window->hide();
    }
}


void Widget::close()
{
    if(isWindow())
    {
        Window::destroyInstance(m_parent.window);
        m_parent.window = nullptr;
        m_flags ^= R64FX_WIDGET_IS_WINDOW;
    }
}


bool Widget::isWindow() const
{
    return m_flags & R64FX_WIDGET_IS_WINDOW;
}


Image* Widget::windowImage() const
{
    if(isWindow())
    {
        return m_parent.window->image();
    }
    else
    {
        return nullptr;
    }
}


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


bool Widget::isObscuredLeft() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_LEFT;
}


bool Widget::isObscuredTop() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_TOP;
}


bool Widget::isObscuredRight() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_RIGHT;
}


bool Widget::isObscuredBottom() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_BOTTOM;
}


bool Widget::isPartiallyObscured() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED;
}


void Widget::setWindowTitle(std::string title)
{
    if(isWindow())
    {
        m_parent.window->setTitle(title);
    }
}


std::string Widget::windowTitle() const
{
    if(isWindow())
    {
        return m_parent.window->title();
    }
    else
    {
        return "";
    }
}


void Widget::mousePressEvent(MouseEvent* event)
{

}


void Widget::mouseReleaseEvent(MouseEvent* event)
{

}


void Widget::mouseMoveEvent(MouseEvent* event)
{

}


void Widget::keyPressEvent(KeyEvent* event)
{

}


void Widget::keyReleaseEvent(KeyEvent* event)
{

}


void Widget::reconfigureEvent(ReconfigureEvent* event)
{
    if(event->sizeChanged())
    {
        if(!m_children.isEmpty())
        {
            for(auto child : m_children)
            {
                unsigned long flags = 0;

                if(child->rect().right()  < 0              ||
                   child->rect().bottom() < 0              ||
                   child->rect().left()   > rect().width() ||
                   child->rect().top()    > rect().height() )
                {
                    flags &= ~R64FX_WIDGET_IS_VISIBLE;
                }
                else
                {
                    flags |= R64FX_WIDGET_IS_VISIBLE;

                    if(child->rect().left() < 0)
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_LEFT;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_LEFT;
                    }

                    if(child->rect().top() < 0)
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_TOP;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_TOP;
                    }

                    if(child->rect().right() > rect().width())
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_RIGHT;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_RIGHT;
                    }

                    if(child->rect().bottom() > rect().height())
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_BOTTOM;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_BOTTOM;
                    }
                }

                child->m_flags =
                    (child->m_flags & ~R64FX_WIDGET_VISIBILITY_MASK) | flags;

                if(child->isVisible())
                {
                    auto p = event->painter();
                    auto offset = p->offset();
                    p->setOffset(offset + child->position());
                    child->reconfigureEvent(event);
                    p->setOffset(offset);
                }
            }//for
        }
    }
}

}//namespace r64fx
