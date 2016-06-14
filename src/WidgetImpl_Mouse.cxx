/* To be included in Widget.cpp */

namespace r64fx{

namespace{

Point<int> g_prev_mouse_position = {0, 0};

MouseButton g_pressed_buttons = MouseButton::None();

/* Widget that currently recieves mouse move events. */
Widget* g_moused_over_widget = nullptr;

/* Widget that currently grabs mouse input. */
Widget* g_mouse_grabber   = nullptr;

}//namespace


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
    return (this == g_mouse_grabber);
}


MouseButton Widget::pressedButtons()
{
    return g_pressed_buttons;
}


void Widget::initMousePressEvent(
    Point<int> event_position,
    MouseButton button,
    bool ignore_grabs,
    bool ignore_self
)
{
    auto dst = mouseGrabber();
    if(dst && !ignore_grabs)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        dst = this;
    }

    MousePressEvent event(event_position, {0, 0}, button);

    if(ignore_self && dst == this)
    {
        dst->childrenMousePressEvent(&event);
    }
    else
    {
        dst->mousePressEvent(&event);
    }
}


void Widget::mousePressEvent(MousePressEvent* event)
{
    childrenMousePressEvent(event);
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


void Widget::initMouseReleaseEvent(
    Point<int> event_position,
    MouseButton button,
    bool ignore_grabs,
    bool ignore_self
)
{
    auto dst = mouseGrabber();
    if(dst && !ignore_grabs)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        dst = this;
    }

    MouseReleaseEvent event(event_position, {0, 0}, button);

    if(ignore_self && dst == this)
    {
        dst->childrenMouseReleaseEvent(&event);
    }
    else
    {
        dst->mouseReleaseEvent(&event);
    }
}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{
    childrenMouseReleaseEvent(event);
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


Widget* Widget::initMouseMoveEvent(
    Point<int> event_position,
    Point<int> event_delta,
    MouseButton pressed_buttons,
    Widget* moused_over_widget,
    bool ignore_grabs,
    bool ignore_self
)
{
    auto dst = Widget::mouseGrabber();
    if(dst && !ignore_grabs)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        Point<int> leaf_offset = {0, 0};
        dst = leafAt(event_position, &leaf_offset);
        event_position -= leaf_offset;
    }

    if(ignore_self && dst == this)
        return dst;

    MouseMoveEvent event(event_position, event_delta, pressed_buttons);
    if(dst != moused_over_widget)
    {
        if(moused_over_widget)
        {
            moused_over_widget->initMouseLeaveEvent();
        }

        if(dst)
        {
            dst->initMouseEnterEvent();
        }
    }

    if(dst)
    {
        dst->mouseMoveEvent(&event);
    }

    return dst;
}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget::initMouseEnterEvent()
{
    m_flags |= R64FX_WIDGET_IS_HOVERED;
    mouseEnterEvent();
}


void Widget::mouseEnterEvent()
{

}


void Widget::initMouseLeaveEvent()
{
    m_flags &= ~R64FX_WIDGET_IS_HOVERED;
    mouseLeaveEvent();
}


void Widget::mouseLeaveEvent()
{

}


bool Widget::isHovered()
{
    return m_flags & R64FX_WIDGET_IS_HOVERED;
}

}//namespace r64fx
