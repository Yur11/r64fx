#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "Painter.hpp"
#include "Program.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

class WindowEvents_Widget : public WindowEvents{
    virtual void resizeEvent(Window* window, int width, int height);

    virtual void mousePressEvent   (Window* window, int x, int y, unsigned int button);
    virtual void mouseReleaseEvent (Window* window, int x, int y, unsigned int button);
    virtual void mouseMoveEvent    (Window* window, int x, int y);

    virtual void keyPressEvent     (Window* window, unsigned int key);
    virtual void keyReleaseEvent   (Window* window, unsigned int key);

    virtual void textInputEvent    (Window* window, const std::string &text, unsigned int key);

    virtual void clipboardDataRecieveEvent
                         (Window* window, ClipboardDataType type, void* data, int size, ClipboardMode mode);

    virtual void clipboardDataTransmitEvent
                         (Window* window, ClipboardDataType type, void** data, int* size, ClipboardMode mode);

    virtual void clipboardMetadataRecieveEvent
                         (Window* window, const ClipboardMetadata &metadata, ClipboardMode mode);

    virtual void closeEvent(Window* window);
};


namespace{
    void set_bits(unsigned long &flags, const bool yes, unsigned long mask)
    {
        if(yes)
            flags |= mask;
        else
            flags &= ~mask;
    }

    MouseButton pressed_buttons = MouseButton::None();

    /* Widget that currently grabs mouse input. */
    Widget* g_mouse_grabber   = nullptr;

    /* Widget that currently has keyboard focus. */
    Widget* g_focus_owner = nullptr;

    Widget* g_anouncer_clipboard       = nullptr;
    Widget* g_anouncer_selection       = nullptr;
    Widget* g_anouncer_drag_and_drop   = nullptr;

    Widget* g_requestor_clipboard      = nullptr;
    Widget* g_requestor_selection      = nullptr;
    Widget* g_requestor_drag_and_drop  = nullptr;

    inline Widget* &anouncer(ClipboardMode mode)
    {
        switch(mode)
        {
            case ClipboardMode::Clipboard:
                return g_anouncer_clipboard;
            case ClipboardMode::Selection:
                return g_anouncer_selection;
            default:
                return g_anouncer_drag_and_drop;
        }
    }

    inline Widget* &requestor(ClipboardMode mode)
    {
        switch(mode)
        {
            case ClipboardMode::Clipboard:
                return g_requestor_clipboard;
            case ClipboardMode::Selection:
                return g_requestor_selection;
            default:
                return g_requestor_drag_and_drop;
        }
    }

    /* Maximum number of individual rectangles
     * that can be repainted after reconf. cycle. */
    constexpr int max_rects = 16;

    /* Collection of data attached to the window.
     * We should be able to cast back and forth
     * between WindowWidgetData and Window::ReconfigureEvent. */
    struct WindowWidgetData : Widget::ReconfigureEvent{

        /* Root widget shown in the window that
         * this context is attached to. */
        Widget*  widget = nullptr;

        /* Painter serving the window. */
        Painter* painter = nullptr;

        /* Current visible rect. passed to widget reconf. method. */
        Rect<int> visible_rect;

        /* List of rectangles to be repainted after reconf. cycle. */
        Rect<int> rects[max_rects];

        /* Number of rectangles that must be repainted. */
        int num_rects = 0;

        /* Used in reconf. logic. */
        bool got_rect = false;
    };


    WindowEvents_Widget g_events;

}//namespace


Widget::Widget(Widget* parent)
{
    setParent(parent);
    setFocusOnClick(true);
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


Widget* Widget::root() const
{
    if(isWindow() || !m_parent.widget)
    {
        return (Widget* const)this;
    }
    else
    {
        return m_parent.widget->root();
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
    if(isWindow() && size == Size<int>(m_parent.window->width(), m_parent.window->height()))
    {
        m_parent.window->resize(size.width(), size.height());
    }
    ResizeEvent event(size);
    resizeEvent(&event);
}


Size<int> Widget::size() const
{
    return m_rect.size();
}


void Widget::setWidth(int width)
{
    m_rect.setWidth(width);
}


int Widget::width() const
{
    return m_rect.width();
}


void Widget::setHeight(int height)
{
    m_rect.setHeight(height);
}


int Widget::height() const
{
    return m_rect.height();
}


Rect<int> Widget::rect() const
{
    return m_rect;
}


Point<int> Widget::toRootCoords(Point<int> point) const
{
    point += position();
    if(isWindow() || m_parent.widget == nullptr)
    {
        return point;
    }
    else
    {
        return m_parent.widget->toRootCoords(point);
    }
}


Rect<int> Widget::toRootCoords(Rect<int> rect) const
{
    rect += position();
    if(isWindow() || m_parent.widget == nullptr)
    {
        return rect;
    }
    else
    {
        return m_parent.widget->toRootCoords(rect);
    }
}


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


void Widget::show()
{
    if(!isWindow())
    {
        auto window = Window::newInstance(
            width(), height(), "", Window::Type::GL
        );
#ifdef R64FX_DEBUG
        if(!window)
        {
            cerr << "Widget: Failed to create window!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto painter = Painter::newInstance(window);
#ifdef R64FX_DEBUG
        if(!painter)
        {
            cerr << "Widget: Failed to create painter!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto d = new(nothrow) WindowWidgetData;
#ifdef R64FX_DEBUG
        if(!d)
        {
            cerr << "Widget: Failed to create WindowWidgetData!\n";
            abort();
        }
#endif//R64FX_DEBUG

        d->widget = this;
        d->painter = painter;

        window->setData(d);

        m_parent.window = window;
        m_flags |= R64FX_WIDGET_IS_WINDOW;
    }
    m_parent.window->show();
    m_parent.window->resize(width(), height());
    update();
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
        auto d = (WindowWidgetData*) m_parent.window->data();

        Painter::deleteInstance(d->painter);
        Window::deleteInstance(m_parent.window);
        delete d;
        m_parent.window = nullptr;
        m_flags &= ~R64FX_WIDGET_IS_WINDOW;
    }
}


Window* Widget::window() const
{
    return (Window*)(isWindow() ? m_parent.window : nullptr);
}


Window* Widget::rootWindow() const
{
    return root()->window();
}


bool Widget::isWindow() const
{
    return m_flags & R64FX_WIDGET_IS_WINDOW;
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


void Widget::grabMouse()
{
    g_mouse_grabber = this;
}


void Widget::ungrabMouse()
{
    g_mouse_grabber = nullptr;
}


Widget* Widget::mouseGrabber()
{
    return g_mouse_grabber;
}


bool Widget::isMouseGrabber() const
{
    return this == g_mouse_grabber;
}


MouseButton Widget::pressedButtons()
{
    return pressed_buttons;
}


void Widget::setFocusOnClick(bool yes)
{
    set_bits(m_flags, yes, R64FX_WIDGET_CLICK_FOCUS);
}


bool Widget::gainsFocusOnClick() const
{
    return m_flags & R64FX_WIDGET_CLICK_FOCUS;
}


void Widget::setFocus()
{
    if(g_focus_owner)
    {
        g_focus_owner->focusOutEvent();
    }
    g_focus_owner = this;
    g_focus_owner->focusInEvent();
}


void Widget::removeFocus()
{
    if(g_focus_owner)
    {
        g_focus_owner->focusOutEvent();
    }
    g_focus_owner = nullptr;
}


Widget* Widget::focusOwner()
{
    return g_focus_owner;
}


bool Widget::hasFocus() const
{
    return this == g_focus_owner;
}


void Widget::startTextInput()
{
    auto widget = root();
    if(widget->isWindow())
    {
        widget->window()->startTextInput();
    }
}


void Widget::stopTextInput()
{
    auto widget = root();
    if(widget->isWindow())
    {
        widget->window()->stopTextInput();
    }
}


bool Widget::doingTextInput()
{
    auto widget = root();
    if(widget->isWindow())
    {
        return widget->window()->doingTextInput();
    }
    else
    {
        return false;
    }
}


void Widget::anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->anounceClipboardData(metadata, mode);
        anouncer(mode) = this;
    }
}


void Widget::requestClipboardMetadata(ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->requestClipboardMetadata(mode);
        requestor(mode) = this;
    }
}


void Widget::requestClipboardData(ClipboardDataType type, ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->requestClipboardData(type, mode);
        requestor(mode) = this;
    }
}


void Widget::update()
{
    m_flags |= R64FX_WIDGET_WANTS_UPDATE;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_CHILD_WANTS_UPDATE))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_UPDATE;
        widget = widget->parent();
    }
}


Painter* Widget::ReconfigureEvent::painter()
{
    auto d = (WindowWidgetData*) this;
    return d->painter;
}


Rect<int> Widget::ReconfigureEvent::visibleRect()
{
    auto d = (WindowWidgetData*) this;
    return d->visible_rect;
}


void process_window_updates(Window* window, void*)
{
    window->makeCurrent();

    auto d        = (WindowWidgetData*) window->data();
    auto painter  = d->painter;
    auto widget   = d->widget;

    if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
    {
        painter->adjustForWindowSize();

        d->num_rects = 0;
        d->visible_rect = {0, 0, widget->width(), widget->height()};
        d->got_rect = false;

        if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
        {
            widget->reconfigureEvent((Widget::ReconfigureEvent*)d);
            painter->repaint();
        }
        else
        {
            widget->reconfigureChildren((Widget::ReconfigureEvent*)d);
            if(d->num_rects > 0)
            {
                for(int i=0; i<d->num_rects; i++)
                {
                    auto rect = d->rects[i];
                    rect = intersection(rect, {0, 0, window->width(), window->height()});
                }

                painter->repaint(
                    d->rects,
                    d->num_rects
                );
            }
        }
    }
}


void Widget::processEvents()
{
    Window::processSomeEvents(&g_events);
    Window::forEach(process_window_updates, nullptr);
}


void Widget::reconfigureEvent(Widget::ReconfigureEvent* event)
{
    reconfigureChildren(event);
}


void Widget::reconfigureChildren(Widget::ReconfigureEvent* event)
{
    auto d = (WindowWidgetData*) event;
    auto parent_visible_rect = d->visible_rect;
    bool got_rect = d->got_rect;

    if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
    {
        if(!d->got_rect)
        {
            if(d->num_rects < max_rects)
            {
                d->rects[d->num_rects] = toRootCoords({0, 0, width(), height()});
                d->num_rects++;
                d->got_rect = true;
            }
        }

        for(auto child : m_children)
        {
            if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                auto visible_rect = intersection(child->rect(), parent_visible_rect);
                if(visible_rect.width() > 0 && visible_rect.height() > 0)
                {
                    child->m_flags |= R64FX_WIDGET_IS_VISIBLE;
                    child->m_flags |= R64FX_WIDGET_WANTS_UPDATE;
                }
                else
                {
                    child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
                }
            }
        }
    }

    for(auto child : m_children)
    {
        if(child->isVisible() && (child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
        {
            auto offset = d->painter->offset();
            d->painter->setOffset(offset + child->position());
            auto visible_rect = intersection(child->rect(), parent_visible_rect);
            d->visible_rect = {0, 0, visible_rect.width(), visible_rect.height()};
            if(child->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                child->reconfigureEvent((ReconfigureEvent*)d);
            }
            else
            {
                child->reconfigureChildren((ReconfigureEvent*)d);
            }
            d->painter->setOffset(offset);
        }
    }

    d->got_rect = got_rect;
    d->visible_rect = parent_visible_rect;
    set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}


void Widget::focusInEvent()
{

}


void Widget::focusOutEvent()
{

}


void WindowEvents_Widget::resizeEvent(Window* window, int width, int height)
{
    auto d = (WindowWidgetData*) window->data();
    d->widget->setSize({width, height});
    d->widget->update();
}


void Widget::resizeEvent(ResizeEvent* event)
{

}


void WindowEvents_Widget::mousePressEvent(Window* window, int x, int y, unsigned int button)
{
    auto d = (WindowWidgetData*) window->data();

    pressed_buttons |= MouseButton(button);
    MousePressEvent event(x, y, MouseButton(button));

    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event.setPosition(
            event.position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mousePressEvent(&event);
    }
    else
    {
        d->widget->mousePressEvent(&event);
    }
}


void Widget::mousePressEvent(MousePressEvent* event)
{
    if(gainsFocusOnClick())
    {
        setFocus();
    }

    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mousePressEvent(event);
            event->setPosition(position);
        }
    }
}


void WindowEvents_Widget::mouseReleaseEvent(Window* window, int x, int y, unsigned int button)
{
    auto d = (WindowWidgetData*) window->data();

    pressed_buttons &= ~MouseButton(button);
    MouseReleaseEvent event(x, y, MouseButton(button));

    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event.setPosition(
            event.position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mouseReleaseEvent(&event);
    }
    else
    {
        d->widget->mouseReleaseEvent(&event);
    }
}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mouseReleaseEvent(event);
            event->setPosition(position);
        }
    }
}


void WindowEvents_Widget::mouseMoveEvent(Window* window, int x, int y)
{
    auto d = (WindowWidgetData*) window->data();

    MouseMoveEvent event(x, y, pressed_buttons);

    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event.setPosition(
            event.position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mouseMoveEvent(&event);
    }
    else
    {
        d->widget->mouseMoveEvent(&event);
    }
}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mouseMoveEvent(event);
            event->setPosition(position);
        }
    }
}


void WindowEvents_Widget::keyPressEvent(Window* window, unsigned int key)
{
    auto d = (WindowWidgetData*) window->data();

    Keyboard::trackModifierPress(key);

    KeyPressEvent event(key);
    if(g_focus_owner)
    {
        g_focus_owner->keyPressEvent(&event);
    }
    else
    {
        d->widget->keyPressEvent(&event);
    }
}


void Widget::keyPressEvent(KeyPressEvent* event)
{

}


void WindowEvents_Widget::keyReleaseEvent(Window* window, unsigned int key)
{
    auto d = (WindowWidgetData*) window->data();

    Keyboard::trackModifierRelease(key);

    KeyReleaseEvent event(key);
    if(g_focus_owner)
    {
        g_focus_owner->keyReleaseEvent(&event);
    }
    else
    {
        d->widget->keyReleaseEvent(&event);
    }
}


void Widget::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void WindowEvents_Widget::textInputEvent(Window* window, const std::string &text, unsigned int key)
{
    auto d = (WindowWidgetData*) window->data();

    Keyboard::trackModifierPress(key);

    TextInputEvent event(text, key);
    if(g_focus_owner)
    {
        g_focus_owner->textInputEvent(&event);
    }
    else
    {
        d->widget->textInputEvent(&event);
    }
}


void Widget::textInputEvent(TextInputEvent* event)
{

}



void WindowEvents_Widget::clipboardDataRecieveEvent
(Window* window, ClipboardDataType type, void* data, int size, ClipboardMode mode)
{
    auto widget = requestor(mode);
    if(widget)
    {
        ClipboardDataRecieveEvent event(mode, type, data, size);
        widget->clipboardDataRecieveEvent(&event);
    }
}


void Widget::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{

}


void WindowEvents_Widget::clipboardDataTransmitEvent
(Window* window, ClipboardDataType type, void** data, int* size, ClipboardMode mode)
{
    auto widget = anouncer(mode);
    if(widget)
    {
        ClipboardDataTransmitEvent event(mode, type, data, size);
        widget->clipboardDataTransmitEvent(&event);
    }
}


void Widget::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{
}


void WindowEvents_Widget::clipboardMetadataRecieveEvent
(Window* window, const ClipboardMetadata &metadata, ClipboardMode mode)
{
    auto widget = requestor(mode);
    if(widget)
    {
        ClipboardMetadataRecieveEvent event(mode, metadata);
        widget->clipboardMetadataRecieveEvent(&event);
    }
}


void Widget::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{

}


void WindowEvents_Widget::closeEvent(Window* window)
{
    auto d = (WindowWidgetData*) window->data();
    d->widget->closeEvent();
}


void Widget::closeEvent()
{

}

}//namespace r64fx
