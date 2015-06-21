#include "Widget.hpp"
#include "Window.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"

#include <algorithm>

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
    if(isWindow())
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::show()\nTrying to show twice!\n";
#endif//R64FX_DEBUG
        return;
    }
    setParent(nullptr);
    m_parent.window = Window::createNew(this);
    m_parent.window->resize(width(), height());
    m_parent.window->show();
}


void Widget::hide()
{
    if(!m_parent.window)
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::hide()\nTrying to hide twice!\n";
#endif//R64FX_DEBUG
        return;
    }
    m_parent.window->hide();
    Window::destroy(m_parent.window);
    m_parent.window = nullptr;
}


bool Widget::isWindow() const
{
    return m_flags & WIDGET_IS_WINDOW;
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


void Widget::resizeEvent(ResizeEvent* event)
{

}

}//namespace r64fx
