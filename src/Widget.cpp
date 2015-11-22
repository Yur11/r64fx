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

#include "Impl_WidgetFlags.hpp"

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


void Widget::resize(Size<int> size)
{
    resize(size.w, size.h);
}


void Widget::resize(int w, int h)
{
    m_rect.setSize(w, h);
    if(isWindow())
    {
        m_parent.window->resize(w, h);
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


void Widget::show()
{
    if(!isWindow())
    {
        m_parent.window = Window::newInstance(
            width(), height(), "", Window::Type::Normal
        );
        m_parent.window->setWidget(this);
        m_flags |= WIDGET_IS_WINDOW;
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
        m_flags ^= WIDGET_IS_WINDOW;
    }
}


bool Widget::isWindow() const
{
    return m_flags & WIDGET_IS_WINDOW;
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
    m_rect.setSize(event->newSize());
}

}//namespace r64fx
