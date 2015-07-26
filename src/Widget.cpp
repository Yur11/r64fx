#include "Widget.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"

#include <map>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using std::cerr;
#endif//R64FX_DEBUG

#include "gui_implementation_iface.hpp"
#include "Impl_WidgetFlags.hpp"

namespace r64fx{

namespace{

std::map<Widget*, Impl::WindowHandle_t> widget_windows;

}


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
        if(m_parent)
        {
            m_parent->m_children.remove(this);
        }
    }
    else
    {
        parent->m_children.append(this);
    }
    m_parent = parent;
}


Widget* Widget::parent() const
{
    return (Widget*)(isWindow() ? nullptr : m_parent);
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
        Impl::resize_window(widget_windows[this], w, h);
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
        Impl::show_window(widget_windows[this]);
    }
    else
    {
        auto window = Impl::init_window_normal(this);
        Impl::resize_window(window, width(), height());
        Impl::show_window(window);
        widget_windows[this] = window;
        m_flags |= WIDGET_IS_WINDOW;
    }
}


void Widget::hide()
{
    if(isWindow())
    {
        Impl::hide_window(widget_windows[this]);
    }
}


void Widget::close()
{
    if(isWindow())
    {
        Impl::free_window(widget_windows[this]);
        widget_windows.erase(this);
        m_flags ^= WIDGET_IS_WINDOW;
    }
}


bool Widget::isWindow() const
{
    return m_flags & WIDGET_IS_WINDOW;
}


void Widget::setWindowTitle(const char* title)
{
    if(isWindow())
    {
        Impl::set_window_title(widget_windows[this], title);
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


void Widget::resizeEvent(ResizeEvent* event)
{

}


void Widget::showEvent(ShowEvent* event)
{
    for(auto w : m_children)
    {
        w->showEvent(event);
    }
}

}//namespace r64fx
