#include "Widget.hpp"
#include "Window.hpp"
#include "Program.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"

#include <algorithm>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using std::cerr;
#endif//R64FX_DEBUG

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


Window* Widget::show()
{
    if(m_host_window)
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::show()\nTrying to show twice!\n";
#endif//R64FX_DEBUG
        return m_host_window;
    }
    setParent(nullptr);
    m_host_window = Program::instance()->createWindow(this);
    m_host_window->resize(width(), height());
    m_host_window->show();
    return m_host_window;
}


void Widget::hide()
{
    if(!m_host_window)
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::hide()\nTrying to hide twice!\n";
#endif//R64FX_DEBUG
        return;
    }
    m_host_window->hide();
    delete m_host_window;
    m_host_window = nullptr;
}


void Widget::resize(int w, int h)
{
    m_rect.setSize(w, h);
    if(m_host_window)
    {
        m_host_window->resize(w, h);
    }
}

}//namespace r64fx
