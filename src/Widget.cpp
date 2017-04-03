#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "Painter.hpp"
#include "Timer.hpp"

#include <limits>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

Timer* g_gui_timer = nullptr;

long int g_window_count = 0;

Point<int> g_prev_mouse_position = {0, 0};

MouseButton g_pressed_buttons = MouseButton::None();

/* Widget that currently recieves mouse move events.
   Needed for doing mouse enter/leave events.
 */
Widget* g_moused_over_widget = nullptr;

/* Widget that currently grabs mouse input. */
Widget* g_mouse_focus_owner   = nullptr;

Widget* g_root_mouse_multi_grabber = nullptr;

std::vector<Widget*> g_mouse_multi_grab_targets;

/* Widget that currently has keyboard focus. */
Widget* g_keyboard_focus_owner = nullptr;

Widget* g_anouncer_clipboard       = nullptr;
Widget* g_anouncer_selection       = nullptr;
Widget* g_dnd_source = nullptr;

Widget* g_requestor_clipboard      = nullptr;
Widget* g_requestor_selection      = nullptr;
Widget* g_dnd_target = nullptr;

Widget* g_requestor_anouncer_stub  = nullptr;

Widget* g_dnd_object = nullptr;


inline Widget* &anouncer(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Clipboard:
            return g_anouncer_clipboard;
        case ClipboardMode::Selection:
            return g_anouncer_selection;
        case ClipboardMode::DragAndDrop:
            return g_dnd_source;
        default:
            return g_requestor_anouncer_stub;
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
        case ClipboardMode::DragAndDrop:
            return g_dnd_target;
        default:
            return g_requestor_anouncer_stub;
    }
}


constexpr int max_rects = 32;

struct WidgetImpl{
    Window* window = nullptr;

    /* Root widget shown in the window that
     * this context is attached to. */
    Widget*  root_widget = nullptr;

    /* Painter serving the window. */
    Painter* painter = nullptr;

    /* List of rectangles to be repainted after update cycle. */
    Rect<int> rects[max_rects];

    /* Number of rectangles that must be repainted. */
    int num_rects = 0;

    /* Used in paint logic. */
    bool got_rect = false;

    Point<int> parent_global_position = {0, 0};

    Rect<int> visible_rect = {0, 0, 0, 0};

    void clip();

    void clipChildren(Widget* parent);

    void repaint();

    void paintChildren(Widget* parent);
};


class WindowEventDispatcher : public WindowEventDispatcherIface{

    virtual void resizeEvent(Window* window, int width, int height)
    {
        auto d = (WidgetImpl*) window->data();
        d->root_widget->setSize({width, height});
        d->root_widget->clip();
        d->root_widget->repaint();
    }


    virtual void mousePressEvent(Window* window, int x, int y, unsigned int button)
    {
        g_pressed_buttons |= MouseButton(button);

        if(g_root_mouse_multi_grabber)
        {
            bool got_widget = false;
            auto mouse_screen_position = Point<int>(x, y) + window->position();
            for(auto widget : g_mouse_multi_grab_targets)
            {
                Widget* root_widget = nullptr;
                auto widget_offset = widget->toRootCoords({0, 0}, &root_widget);

                auto widget_root_window = root_widget->window();
                if(!widget_root_window)
                    continue;

                Rect<int> rect(widget_root_window->position() + widget_offset, widget->size());
                if(rect.overlaps(mouse_screen_position))
                {
                    Point<int> event_position = mouse_screen_position - widget_root_window->position();

                    MousePressEvent event(event_position, {0, 0}, MouseButton(button));
                    widget->mousePressEvent(&event);
                    got_widget = true;
                    break;
                }
            }

            if(!got_widget)
            {
                g_root_mouse_multi_grabber->clickedElsewhereEvent();
            }
        }
        else
        {
            Point<int> event_position(x, y);

            auto dst = g_mouse_focus_owner;
            if(dst)
            {
                event_position -= dst->toRootCoords(Point<int>(0, 0));
            }
            else
            {
                auto d = (WidgetImpl*) window->data();
                dst = d->root_widget;
            }

            MousePressEvent event(event_position, {0, 0}, MouseButton(button));
            dst->mousePressEvent(&event);
        }

        g_prev_mouse_position = Point<int>(x, y);
    }


    virtual void mouseReleaseEvent(Window* window, int x, int y, unsigned int button)
    {
        g_pressed_buttons &= ~MouseButton(button);

        if(g_root_mouse_multi_grabber)
        {
            auto mouse_screen_position = Point<int>(x, y) + window->position();
            for(auto widget : g_mouse_multi_grab_targets)
            {
                Widget* root_widget = nullptr;
                auto widget_offset = widget->toRootCoords({0, 0}, &root_widget);

                auto widget_root_window = root_widget->window();
                if(!widget_root_window)
                    continue;

                Rect<int> rect(widget_root_window->position() + widget_offset, widget->size());
                if(rect.overlaps(mouse_screen_position))
                {
                    Point<int> event_position = mouse_screen_position - widget_root_window->position();

                    auto dst = g_mouse_focus_owner;
                    if(dst)
                    {
                        event_position -= dst->toRootCoords(Point<int>(0, 0));
                    }
                    else
                    {
                        dst = widget;
                    }

                    MouseReleaseEvent event(event_position, {0, 0}, MouseButton(button));
                    dst->mouseReleaseEvent(&event);

                    break;
                }
            }
        }
        else
        {
            Point<int> event_position(x, y);

            auto dst = g_mouse_focus_owner;
            if(dst)
            {
                event_position -= dst->toRootCoords(Point<int>(0, 0));
            }
            else
            {
                auto d = (WidgetImpl*) window->data();
                dst = d->root_widget;
            }

            MouseReleaseEvent event(event_position, {0, 0}, MouseButton(button));
            dst->mouseReleaseEvent(&event);
        }
    }


    virtual void mouseMoveEvent(Window* window, int x, int y)
    {
        Widget* root_dst = nullptr;
        Point<int> event_position(x, y);
        Point<int> event_root_position(x, y);
        Point<int> event_delta = event_position - g_prev_mouse_position;

        if(g_root_mouse_multi_grabber)
        {
            auto mouse_screen_position = event_position + window->position();
            for(auto widget : g_mouse_multi_grab_targets)
            {
                Widget* root_widget = nullptr;
                auto widget_offset = widget->toRootCoords({0, 0}, &root_widget);

                auto widget_root_window = root_widget->window();
                if(!widget_root_window)
                    continue;

                Rect<int> rect(widget_root_window->position() + widget_offset, widget->size());
                if(rect.overlaps(mouse_screen_position))
                {
                    root_dst = widget;
                    event_position = mouse_screen_position - widget_root_window->position();
                    break;
                }
            }
        }
        else
        {
            auto d = (WidgetImpl*) window->data();
            root_dst = d->root_widget;
        }

        if(root_dst)
        {
            auto dst = g_mouse_focus_owner;
            if(dst)
            {
                event_position -= dst->toRootCoords(Point<int>(0, 0));
            }
            else
            {
                Point<int> leaf_offset = {0, 0};
                dst = root_dst->leafAt(event_position, &leaf_offset);
                event_position -= leaf_offset;
            }

            MouseMoveEvent event(event_position, event_delta, g_pressed_buttons);
            if(dst != g_moused_over_widget)
            {
                if(g_moused_over_widget)
                {
                    g_moused_over_widget->m_flags &= ~R64FX_WIDGET_IS_HOVERED;
                    g_moused_over_widget->mouseLeaveEvent();
                }

                if(dst)
                {
                    dst->m_flags |= R64FX_WIDGET_IS_HOVERED;
                    dst->mouseEnterEvent();
                }
            }

            if(dst)
            {
                g_moused_over_widget = dst;
                dst->mouseMoveEvent(&event);
            }
        }

        g_prev_mouse_position = event_root_position;
    }


    virtual void mouseEnterEvent(Window* window)
    {
        //Just look for the first mouse move event!
    }


    virtual void mouseLeaveEvent(Window* window)
    {
//         if(g_moused_over_widget)
//         {
//             cout << "Window Leave!\n";
//             g_moused_over_widget->initMouseLeaveEvent();
//         }
    }


    virtual void keyPressEvent(Window* window, unsigned int key)
    {
        Keyboard::trackModifierPress(key);

        KeyPressEvent event(key);
        if(g_keyboard_focus_owner)
        {
            g_keyboard_focus_owner->keyPressEvent(&event);
        }
        else
        {
            auto d = (WidgetImpl*) window->data();
            d->root_widget->keyPressEvent(&event);
        }
    }


    virtual void keyReleaseEvent(Window* window, unsigned int key)
    {
        Keyboard::trackModifierRelease(key);

        KeyReleaseEvent event(key);
        if(g_keyboard_focus_owner)
        {
            g_keyboard_focus_owner->keyReleaseEvent(&event);
        }
        else
        {
            auto d = (WidgetImpl*) window->data();
            d->root_widget->keyReleaseEvent(&event);
        }
    }


    virtual void textInputEvent(Window* window, const std::string &text, unsigned int key)
    {
        Keyboard::trackModifierPress(key);

        TextInputEvent event(text, key);
        if(g_keyboard_focus_owner)
        {
            g_keyboard_focus_owner->textInputEvent(&event);
        }
        else
        {
            auto d = (WidgetImpl*) window->data();
            d->root_widget->textInputEvent(&event);
        }
    }


    virtual void clipboardDataRecieveEvent(
        Window* window, ClipboardDataType type, ClipboardMode mode, void* data, int size
    )
    {
        auto widget = requestor(mode);
        if(widget)
        {
            ClipboardDataRecieveEvent event(mode, type, data, size);
            widget->clipboardDataRecieveEvent(&event);
        }
    }


    virtual void clipboardDataTransmitEvent(
        Window* window, ClipboardDataType type, ClipboardMode mode,
        void (*callback)(Window* window, void* data, int size)
    )
    {
        auto widget = anouncer(mode);
        if(widget)
        {
            ClipboardDataTransmitEvent event(mode, type, callback, window);
            widget->clipboardDataTransmitEvent(&event);
        }
    }


    virtual void clipboardMetadataRecieveEvent(
        Window* window, const ClipboardMetadata &metadata, ClipboardMode mode
    )
    {
        auto widget = requestor(mode);
        if(widget)
        {
            ClipboardMetadataRecieveEvent event(mode, metadata);
            widget->clipboardMetadataRecieveEvent(&event);
        }
    }


    virtual void dndMoveEvent(Window* window, int x, int y, const ClipboardMetadata& metadata, bool &accepted)
    {
        auto d = (WidgetImpl*) window->data();

        Point<int> event_position(x, y);
        Point<int> leaf_offset = {0, 0};
        Widget* dst = d->root_widget->leafAt(event_position, &leaf_offset);
        event_position -= leaf_offset;

        while(dst && !dst->dndEnabled())
        {
            dst = dst->parent();
        }

        if(dst)
        {
            g_dnd_target = dst;
            DndMoveEvent event(event_position, metadata, accepted);
            dst->dndMoveEvent(&event);
        }
    }


    virtual void dndDropEvent(Window* window, const ClipboardMetadata& metadata, ClipboardDataType &data_type, bool &accepted)
    {
        if(g_dnd_target)
        {
            DndDropEvent event(metadata, data_type, accepted);
            g_dnd_target->dndDropEvent(&event);
            if(!accepted)
            {
                g_dnd_target = nullptr;
            }
        }
    }


    virtual void dndLeaveEvent(Window* window)
    {
        if(g_dnd_target)
        {
            DndLeaveEvent event;
            g_dnd_target->dndLeaveEvent(&event);
            g_dnd_target = nullptr;
        }
    }


    virtual void dndReleaseEvent()
    {
        g_pressed_buttons = MouseButton::None();//?
        Widget::releaseMouseFocus();
        if(g_dnd_object && g_dnd_source)
        {
            g_dnd_object->closeWindow();
            DndReleaseEvent event(g_dnd_object);
            g_dnd_source->dndReleaseEvent(&event);
            g_dnd_object = nullptr;

        }
    }


    virtual void dndFinishedEvent()
    {
        g_dnd_source = nullptr;
        g_dnd_target = nullptr;
    }

    virtual void closeEvent(Window* window)
    {
        auto d = (WidgetImpl*) window->data();
        d->root_widget->closeEvent();
    }
};


Widget::Widget(Widget* parent)
{
    setParent(parent);
    m_flags |= R64FX_WIDGET_IS_VISIBLE;
}


Widget::~Widget()
{
    if(!isWindow())
        setParent(nullptr);

    while(!m_children.isEmpty())
    {
        auto child = m_children.last();
        child->setParent(nullptr);
    }

    if(this == g_moused_over_widget)
        g_moused_over_widget = nullptr;

    if(this == g_mouse_focus_owner)
        g_mouse_focus_owner = nullptr;

    if(this == g_root_mouse_multi_grabber)
        g_root_mouse_multi_grabber = nullptr;

    if(this == g_keyboard_focus_owner)
        g_keyboard_focus_owner = nullptr;

    if(this == g_anouncer_clipboard)
        g_anouncer_clipboard = nullptr;

    if(this == g_anouncer_selection)
        g_anouncer_selection = nullptr;

    if(this == g_dnd_source)
        g_dnd_source = nullptr;

    if(this == g_requestor_clipboard)
        g_requestor_clipboard = nullptr;

    if(this == g_requestor_selection)
        g_requestor_selection = nullptr;

    if(this == g_dnd_target)
        g_dnd_target = nullptr;
}


void Widget::setParent(Widget* parent, bool insert_after, Widget* existing_child)
{
    if(isWindow())
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::setParent()\nTrying to set parent on a window!\n";
#endif//R64FX_DEBUG
        abort();
    }

    if(m_parent.widget)
    {
        if(isShownInWindow())
        {
            WidgetRemovedFromWindowEvent event(rootWindow(), textureManager());
            removedFromWindowEvent(&event);
            m_flags &= ~R64FX_WIDGET_BELONGS_TO_WINDOW;
        }
        m_parent.widget->m_children.remove(this);
    }

    if(parent)
    {
        if(existing_child && existing_child->parent() == parent)
        {
            if(insert_after)
                parent->m_children.insertAfter(existing_child, this);
            else
                parent->m_children.insertBefore(existing_child, this);
        }
        else
        {
            if(insert_after)
                parent->m_children.append(this);
            else
                parent->m_children.preppend(this);
        }

        if(parent->isShownInWindow())
        {
            m_flags |= R64FX_WIDGET_BELONGS_TO_WINDOW;
            auto parent_window = parent->rootWindow();
            WidgetAddedToWindowEvent event(parent_window, parent->textureManager());
            addedToWindowEvent(&event);
        }
    }
    m_parent.widget = parent;
}


Widget* Widget::parent() const
{
    return (Widget*)(isWindow() ? nullptr : m_parent.widget);
}


void Widget::preppend(Widget* child)
{
    if(!child)
        return;

    child->setParent(this, false);
}


void Widget::append(Widget* child)
{
    if(!child)
        return;

    child->setParent(this, true);
}


void Widget::insertBefore(Widget* existing_child, Widget* child)
{
    if(!child || !existing_child || existing_child->parent() != this)
        return;

    child->setParent(this, false, existing_child);
}


void Widget::insertAfter(Widget* existing_child, Widget* child)
{
    if(!child || !existing_child || existing_child->parent() != this)
        return;

    child->setParent(this, true, existing_child);
}


WidgetIterator Widget::begin() const
{
    return m_children.begin();
}


WidgetIterator Widget::end() const
{
    return m_children.end();
}


bool Widget::hasChildren() const
{
    return !m_children.isEmpty();
}


Widget* Widget::firstChild() const
{
    return m_children.first();
}


Widget* Widget::popFirstChild()
{
    if(m_children.isEmpty())
        return nullptr;
    auto child = m_children.first();
    child->setParent(nullptr);
    return child;
}


Widget* Widget::lastChild() const
{
    return m_children.first();
}


Widget* Widget::popLastChild()
{
    if(m_children.isEmpty())
        return nullptr;
    auto child = m_children.last();
    child->setParent(nullptr);
    return child;
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
    m_position = pos;
}


Point<int> Widget::position() const
{
    return m_position;
}


void Widget::setX(int x)
{
    m_position.setX(x);
}


int Widget::x() const
{
    return m_position.x();
}


void Widget::setY(int y)
{
    m_position.setY(y);
}


int Widget::y() const
{
    return m_position.y();
}


void Widget::setSize(Size<int> size, bool send_event)
{
    auto old_size = m_size;
    m_size = size;
    if(isWindow() && size != m_parent.window->size())
    {
        m_parent.window->setSize(size);
    }

    if(send_event)
    {
        WidgetResizeEvent event(old_size, size);
        resizeEvent(&event);
    }
}


Size<int> Widget::size() const
{
    return m_size;
}


void Widget::setWidth(int width, bool send_event)
{
    setSize({width, height()}, send_event);
}


int Widget::width() const
{
    return m_size.width();
}


void Widget::setHeight(int height, bool send_event)
{
    setSize({width(), height}, send_event);
}


int Widget::height() const
{
    return m_size.height();
}


Widget* Widget::leafAt(Point<int> position, Point<int>* offset)
{
    Widget* leaf = this;
    Point<int> leaf_offset = {0, 0};
    bool got_leaf = true;
    while(got_leaf)
    {
        leaf_offset += leaf->contentOffset();

        got_leaf = false;
        for(auto child : leaf->m_children)
        {
            if(Rect<int>(child->position(), child->size()).overlaps(position - leaf_offset))
            {
                got_leaf = true;
                leaf_offset += child->position();
                leaf = child;
                break;
            }
        }
    }

    if(offset)
    {
        offset[0] = leaf_offset;
    }
    return leaf;
}


Point<int> Widget::toRootCoords(Point<int> point, Widget** root)
{
    auto widget = this;
    for(;;)
    {
        point += widget->position();
        if(widget->parent())
        {
            point += widget->parent()->contentOffset();
            widget = widget->parent();
            continue;
        }
        else
        {
            break;
        }
    }

    if(root)
    {
        root[0] = widget;
    }

    return point;
}


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


Point<int> Widget::contentOffset()
{
    return {0, 0};
}


Rect<int> Widget::childrenBoundingRect() const
{
    int min_left   = std::numeric_limits<int>::max();
    int min_top    = std::numeric_limits<int>::max();
    int max_right  = 0;
    int max_bottom = 0;

    for(auto child : m_children)
    {
        Rect<int> rect(child->position(), child->size());

        if(rect.left() < min_left)
            min_left = rect.left();

        if(rect.top() < min_top)
            min_top = rect.top();

        if(rect.right() > max_right)
            max_right = rect.right();

        if(rect.bottom() > max_bottom)
            max_bottom = rect.bottom();
    }

    return Rect<int>(min_left, min_top, max_right - min_left, max_bottom - min_top);
}


void Widget::setOrientation(Orientation orientation)
{
    if(orientation == Orientation::Vertical)
        m_flags |= R64FX_WIDGET_IS_VERTICAL;
    else
        m_flags &= ~R64FX_WIDGET_IS_VERTICAL;
}


Orientation Widget::orientation() const
{
    return (m_flags & R64FX_WIDGET_IS_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal;
}


void Widget::openWindow(
    Window::WmType  wm_type,
    Window::Type    type,
    Window*         modal_parent
)
{
    /* Create new window if needed. */
    if(!isWindow())
    {
        if(width() <= 0)
            setWidth(100);

        if(height() <= 0)
            setHeight(100);

        auto window = Window::newInstance(
            width(), height(), type, wm_type
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

        auto d = new(std::nothrow) WidgetImpl;
        if(d)
        {
            d->window = window;
            d->root_widget = this;
            d->painter = painter;

            window->setData(d);

            m_parent.window = window;
            m_flags |= R64FX_WIDGET_IS_WINDOW;

            g_window_count++;

            /* Setup gui timer if needed. */
            if(!g_gui_timer)
            {
                g_gui_timer = new (std::nothrow) Timer;
                if(g_gui_timer)
                {
                    g_gui_timer->setInterval(5000 * 1000);
                    g_gui_timer->onTimeout([](Timer* timer, void* data){

                        WindowEventDispatcher events;
                        Window::processSomeEvents(&events);

                        Window::forEach([](Window* window, void* data){
                            auto impl = (WidgetImpl*) window->data();
                            impl->clip();
                            impl->repaint();
                        }, nullptr);

                    }, nullptr);
                    g_gui_timer->start();
                }
#ifdef R64FX_DEBUG
                else
                {
                    cerr << "Widget: Failed to create gui timer!\n";
                }
#endif//R64FX_DEBUG
            }

            window->setSize(size());
            if(modal_parent)
                m_parent.window->setModalTo(modal_parent);
            window->show();
            m_flags |= R64FX_WIDGET_BELONGS_TO_WINDOW;
            WidgetAddedToWindowEvent event(window, painter);
            addedToWindowEvent(&event);
            clip();
            repaint();
        }
#ifdef R64FX_DEBUG
        else
        {
            cerr << "Widget: Failed to create WidgetImpl!\n";
        }
#endif//R64FX_DEBUG
    }
}


void Widget::closeWindow()
{
    if(isWindow())
    {
        WidgetRemovedFromWindowEvent event(m_parent.window, textureManager());
        removedFromWindowEvent(&event);

        auto d = (WidgetImpl*) m_parent.window->data();
        auto painter = d->painter;
        Painter::deleteInstance(painter);
        delete d;

        m_parent.window->hide();
        Window::deleteInstance(m_parent.window);
        m_parent.window = nullptr;
        m_flags &= ~R64FX_WIDGET_IS_WINDOW;

        g_window_count--;
        if(g_window_count == 0)
        {
            if(g_gui_timer)
            {
                g_gui_timer->stop();
                delete g_gui_timer;
            }
            else
            {
#ifdef R64FX_DEBUG
                cerr << "Widget: gui timer is null!\n";
#endif//R64FX_DEBUG
            }
        }
#ifdef R64FX_DEBUG
        else if(g_window_count < 0)
        {
            cerr << "Widget: Window count is less than zero!\n";
        }
#endif//R64FX_DEBUG
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


bool Widget::isShownInWindow() const
{
    return m_flags & R64FX_WIDGET_BELONGS_TO_WINDOW;
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


void Widget::grabMouseFocus()
{
    g_mouse_focus_owner = this;
}


void Widget::releaseMouseFocus()
{
    g_mouse_focus_owner = nullptr;
}


Widget* Widget::mouseFocusOwner()
{
    return g_mouse_focus_owner;
}


bool Widget::isMouseFocusOwner() const
{
    return (this == g_mouse_focus_owner);
}


void Widget::grabMouseForMultipleWidgets()
{
    auto root_window = rootWindow();
    if(!root_window)
        return;

    root_window->grabMouseFocus();
    g_root_mouse_multi_grabber = this;
}


void Widget::ungrabMouseForMultipleWidgets()
{
    auto root_window = rootWindow();
    if(!root_window)
        return;

    root_window->releaseMouseFocus();
    g_root_mouse_multi_grabber = nullptr;
}


bool Widget::wantsMultiGrabs(bool yes)
{
    if(yes)
    {
        if(!wantsMultiGrabs())
        {
            g_mouse_multi_grab_targets.push_back(this);
        }
        m_flags |= R64FX_WIDGET_WANTS_MULTI_GRABS;
    }
    else
    {
        if(wantsMultiGrabs())
        {
            auto it = g_mouse_multi_grab_targets.begin();
            while(it != g_mouse_multi_grab_targets.end())
            {
                if(*it == this)
                {
                    g_mouse_multi_grab_targets.erase(it);
                    break;
                }
            }
        }
        m_flags &= ~R64FX_WIDGET_WANTS_MULTI_GRABS;
    }
    return yes;
}


bool Widget::wantsMultiGrabs()
{
    return m_flags & R64FX_WIDGET_WANTS_MULTI_GRABS;
}


Widget* Widget::mouseMultiGrabber()
{
    return g_root_mouse_multi_grabber;
}


MouseButton Widget::pressedButtons()
{
    return g_pressed_buttons;
}


bool Widget::isHovered()
{
    return m_flags & R64FX_WIDGET_IS_HOVERED;
}


void Widget::grabKeyboardFocus()
{
    if(g_keyboard_focus_owner)
    {
        g_keyboard_focus_owner->focusOutEvent();
    }
    g_keyboard_focus_owner = this;
    g_keyboard_focus_owner->focusInEvent();
}


void Widget::releaseKeyboardFocus()
{
    if(g_keyboard_focus_owner)
    {
        g_keyboard_focus_owner->focusOutEvent();
    }
    g_keyboard_focus_owner = nullptr;
}


Widget* Widget::keyboardFocusOwner()
{
    return g_keyboard_focus_owner;
}


bool Widget::hasKeyboardFocus() const
{
    return this == g_keyboard_focus_owner;
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
        requestor(mode) = this;
        win->requestClipboardMetadata(mode);
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


void Widget::startDrag(const ClipboardMetadata &metadata, Widget* dnd_object, Point<int> anchor)
{
    if(g_dnd_object)
        return;

    auto root_window = rootWindow();
    if(root_window)
    {
        g_dnd_object = dnd_object;
        g_dnd_source = this;
        if(!dnd_object->isWindow())
        {
            dnd_object->openWindow(Window::WmType::DND, Window::Type::Image);
        }
        root_window->startDrag(metadata, dnd_object->window(), anchor.x(), anchor.y());
    }
}


bool Widget::dndEnabled(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_WANTS_DND;
    else
        m_flags &= ~R64FX_WIDGET_WANTS_DND;
    return yes;
}


bool Widget::dndEnabled() const
{
    return m_flags & R64FX_WIDGET_WANTS_DND;
}


WidgetImpl* WidgetPaintEvent::impl() const
{
    return m_impl;
}


Painter* WidgetPaintEvent::painter() const
{
    return m_impl->painter;
}


void Widget::clip()
{
    m_flags |= R64FX_WIDGET_WANTS_CLIPPING;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_CLIP_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_CLIPPING;
        widget = widget->parent();
    }
}


void WidgetImpl::clip()
{
    if(root_widget->m_flags & R64FX_WIDGET_CLIP_FLAGS)
    {
        parent_global_position = {0, 0};
        visible_rect = Rect<int>({0, 0}, root_widget->size());
        clipChildren(root_widget);
    }
    root_widget->m_flags &= ~R64FX_WIDGET_CLIP_FLAGS;
}


void WidgetImpl::clipChildren(Widget* parent)
{
    Point<int> offset = parent->contentOffset();

    for(auto child : *parent)
    {
        if(parent->m_flags & R64FX_WIDGET_WANTS_CLIPPING)
            child->m_flags |= R64FX_WIDGET_WANTS_CLIPPING;

        if(!(child->m_flags & R64FX_WIDGET_CLIP_FLAGS))
            continue;

        Point<int> old_parent_global_position = parent_global_position;
        Rect<int> old_visible_rect = visible_rect;

        Point<int> child_global_position = parent_global_position + offset + child->position();

        Rect<int> child_global_rect(child_global_position, child->size());
        Rect<int> rect = intersection(child_global_rect, visible_rect);

        if(child->m_flags & R64FX_WIDGET_WANTS_CLIPPING)
        {
            WidgetClipEvent event(rect - child_global_position);
            child->clipEvent(&event);

            if(rect.width() > 0 && rect.height() > 0)
            {
                child->m_flags |= R64FX_WIDGET_IS_VISIBLE;

                parent_global_position = child_global_position;
                visible_rect = rect;
                clipChildren(child);
            }
            else
            {
                child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
            }
        }
        else
        {
            parent_global_position = child_global_position;
            visible_rect = rect;
            clipChildren(child);
        }

        parent_global_position = old_parent_global_position;
        visible_rect = old_visible_rect;

        child->m_flags &= ~R64FX_WIDGET_CLIP_FLAGS;
    }
}


void Widget::repaint()
{
    m_flags |= R64FX_WIDGET_WANTS_REPAINT;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_REPAINT_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_REPAINT;
        widget = widget->parent();
    }
}


/* Do a repaint cycle.

   For every widget that has R64FX_WIDGET_WANTS_REPAINT flag set,
   paintEvent() handler must be called.
 */
void WidgetImpl::repaint()
{
    window->makeCurrent();

    auto &flags = root_widget->m_flags;

    if(flags & R64FX_WIDGET_REPAINT_FLAGS)
    {
        painter->adjustForWindowSize();

        num_rects = 0;
        got_rect = false;

        if(flags & R64FX_WIDGET_WANTS_REPAINT) //The root widget wants repaint.
        {
            //Paint whole window surface.
            WidgetPaintEvent event(this);
            painter->resetPaintGroups();
            painter->beginPaintGroup();
            root_widget->paintEvent(&event);
            painter->endPaintGroup();
            painter->repaint();
        }
        else
        {
            //Paint only certain parts of the window surface.
            paintChildren(root_widget);
            if(num_rects > 0)
            {
                for(int i=0; i<num_rects; i++)
                {
                    auto rect = rects[i];
                    rect = intersection(rect, {0, 0, window->width(), window->height()});
                }

                painter->repaint(
                    rects,
                    num_rects
                );
            }
        }

        flags &= ~R64FX_WIDGET_REPAINT_FLAGS;
    }
}


PainterTextureManager* Widget::textureManager()
{
    Widget* root_widget = nullptr;
    toRootCoords({0, 0}, &root_widget);
    auto root_window = root_widget->window();
    if(!root_window)
        return nullptr;
    auto d = (WidgetImpl*) root_window->data();
    return d->painter;
}


void Widget::deleteChildren()
{
    while(!m_children.isEmpty())
    {
        auto child = m_children.first();
        child->setParent(nullptr);
        delete child;
    }
}


void Widget::childrenPaintEvent(WidgetPaintEvent* event)
{
    event->impl()->paintChildren(this);
}


void Widget::childrenAddedToWindowEvent(WidgetAddedToWindowEvent* event)
{
    for(auto child : m_children)
    {
        child->m_flags |= R64FX_WIDGET_BELONGS_TO_WINDOW;
        child->addedToWindowEvent(event);
    }
}


void Widget::childrenRemovedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    for(auto child : m_children)
    {
        child->removedFromWindowEvent(event);
        child->m_flags &= ~R64FX_WIDGET_BELONGS_TO_WINDOW;
    }
}


bool Widget::childrenMousePressEvent(MousePressEvent* event)
{
    Point<int> event_pos = event->position() - contentOffset();
    for(auto child : m_children)
    {
        if(Rect<int>(child->position(), child->size()).overlaps(event_pos))
        {
            Point<int> old_pos = event->position();
            event->setPosition(event_pos - child->position());
            child->mousePressEvent(event);
            event->setPosition(old_pos);
            return true;
        }
    }
    return false;
}



bool Widget::childrenMouseReleaseEvent(MouseReleaseEvent* event)
{
    Point<int> event_pos = event->position() - contentOffset();
    for(auto child : m_children)
    {
        if(Rect<int>(child->position(), child->size()).overlaps(event_pos))
        {
            Point<int> old_pos = event->position();
            event->setPosition(event_pos - child->position());
            child->mouseReleaseEvent(event);
            event->setPosition(old_pos);
            return true;
        }
    }
    return false;
}



/* Recursivly paint child widgets.

   If R64FX_CHILD_WANTS_REPAINT flag is set on one of the children,
   simply does a recursive call of paintChildren().

   If R64FX_WIDGET_WANTS_REPAINT flag is set,
   calls child's paintEvent().
   Within it's paintEvent() implementation the child may call base class implementation
   of paintEvent() that simply calls paintChildren().

   See WidgetFlags.hpp

   Doing this recursivly allows a widget to do some painting, make it's children paint themselves
   and then do some more painting on top.
 */
void WidgetImpl::paintChildren(Widget* parent)
{
    bool parent_got_rect = got_rect;

    auto &parent_flags = parent->m_flags;

    if(parent_flags & R64FX_WIDGET_WANTS_REPAINT)
    {
        for(auto child : *parent)
        {
            child->m_flags |= R64FX_WIDGET_WANTS_REPAINT;
        }
    }

    for(auto child : *parent)
    {
        if(!(child->m_flags & R64FX_WIDGET_REPAINT_FLAGS) || !child->isVisible())
            continue;

        auto &child_flags = child->m_flags;

        /* Children should use painter in their local coord. system. */
        auto old_offset = painter->offset();
        painter->setOffset(old_offset + child->position() + parent->contentOffset());

        if(child_flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            got_rect = true; //The child will read this value as it's parent_got_rect
                             //and will know not to add any rects of it's own.

            WidgetPaintEvent event(this);
            painter->beginPaintGroup();
            child->paintEvent(&event);
            painter->endPaintGroup();

            if(!parent_got_rect)
            {
                //Painter offset is equal to the window coord. of the child widget.
                Rect<int> rect(painter->offset(), child->size());
                if(num_rects < max_rects)
                {
                    rects[num_rects] = rect;
                    num_rects++;
                }
            }
        }
        else
        {
            paintChildren(child);
        }

        painter->setOffset(old_offset);

        child_flags &= ~R64FX_WIDGET_REPAINT_FLAGS;
    }

    got_rect = parent_got_rect;
}


void Widget::paintEvent(WidgetPaintEvent* event)
{
    childrenPaintEvent(event);
}


void Widget::resizeEvent(WidgetResizeEvent* event)
{

}


void Widget::clipEvent(WidgetClipEvent* event)
{

}


void Widget::addedToWindowEvent(WidgetAddedToWindowEvent* event)
{
    childrenAddedToWindowEvent(event);
}


void Widget::removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    childrenRemovedFromWindowEvent(event);
}


void Widget::mousePressEvent(MousePressEvent* event)
{
    childrenMousePressEvent(event);
}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{
    childrenMouseReleaseEvent(event);
}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget::clickedElsewhereEvent()
{

}


void Widget::mouseEnterEvent()
{

}


void Widget::mouseLeaveEvent()
{

}


void Widget::focusInEvent()
{

}


void Widget::focusOutEvent()
{

}


void Widget::keyPressEvent(KeyPressEvent* event)
{

}


void Widget::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void Widget::textInputEvent(TextInputEvent* event)
{

}


void Widget::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{

}


void Widget::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{

}


void Widget::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{

}


void Widget::dndMoveEvent(DndMoveEvent* event)
{

}


void Widget::dndDropEvent(DndDropEvent* event)
{

}


void Widget::dndLeaveEvent(DndLeaveEvent* event)
{

}


void Widget::dndReleaseEvent(DndReleaseEvent* event)
{

}


void Widget::dndFinishedEvent(DndFinishedEvent* event)
{

}


void Widget::closeEvent()
{
    closeWindow();
}

}//namespace r64fx
