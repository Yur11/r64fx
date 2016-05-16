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


bool Widget::grabsMouseOnClick(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_GRABS_MOUSE_ON_CLICK;
    else
        m_flags &= ~R64FX_WIDGET_GRABS_MOUSE_ON_CLICK;
    return yes;
}


bool Widget::grabsMouseOnClick() const
{
    return m_flags & R64FX_WIDGET_GRABS_MOUSE_ON_CLICK;
}


bool Widget::ungrabsMouseOnRelease(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_UNGRABS_MOUSE_ON_RELEASE;
    else
        m_flags &= ~R64FX_WIDGET_UNGRABS_MOUSE_ON_RELEASE;
    return yes;
}


bool Widget::ungrabsMouseOnRelease() const
{
    return m_flags & R64FX_WIDGET_UNGRABS_MOUSE_ON_RELEASE;
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
        Point<int> offset  = {0, 0};
        dst = leafAt(event_position, &offset);
        event_position -= offset;
    }

    if(!ignore_grabs && dst->grabsMouseOnClick())
    {
        dst->grabMouse();
    }

    if(dst->getsFocusOnClick())
    {
        dst->setFocus();
    }

    if(ignore_self && dst == this)
        return;

    MousePressEvent event(event_position, {0, 0}, button);
    dst->mousePressEvent(&event);
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
        Point<int> leaf_offset = {0, 0};
        dst = leafAt(event_position, &leaf_offset);
        event_position -= leaf_offset;
    }

    if(!ignore_grabs && dst->ungrabsMouseOnRelease())
    {
        ungrabMouse();
    }

    if(ignore_self && dst == this)
        return;

    MouseReleaseEvent event(event_position, {0, 0}, button);
    dst->mouseReleaseEvent(&event);
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
        dst->initMouseEnterEvent();
    }
    dst->mouseMoveEvent(&event);

    return dst;
}


void Widget::initMouseEnterEvent()
{
    m_flags |= R64FX_WIDGET_IS_HOVERED;
    mouseEnterEvent();
}


void Widget::initMouseLeaveEvent()
{
    m_flags &= ~R64FX_WIDGET_IS_HOVERED;
    mouseLeaveEvent();
}


bool Widget::isHovered()
{
    return m_flags & R64FX_WIDGET_IS_HOVERED;
}

}//namespace r64fx
