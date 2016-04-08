/* To be included in Widget.cpp */

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


void Widget::grabMouseOnClick(bool yes)
{
    set_bits(m_flags, yes, R64FX_WIDGET_GRABS_MOUSE_ON_CLICK);
}


bool Widget::grabsMouseOnClick() const
{
    return m_flags & R64FX_WIDGET_GRABS_MOUSE_ON_CLICK;
}


void Widget::initMousePressEvent(Point<int> event_position, MouseButton button)
{
    g_prev_mouse_position = event_position;

    pressed_buttons |= button;

    auto dst = mouseGrabber();
    if(dst)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        Point<int> offset  = {0, 0};
        dst = leafAt(event_position, &offset);
        event_position -= offset;
    }

    if(dst->grabsMouseOnClick())
    {
        dst->grabMouse();
    }

    if(dst->gainsFocusOnClick())
    {
        dst->setFocus();
    }

    MousePressEvent event(event_position, {0, 0}, button);
    dst->mousePressEvent(&event);
}


void Widget::initMouseReleaseEvent(Point<int> event_position, MouseButton button)
{
    g_prev_mouse_position = event_position;

    pressed_buttons &= ~button;

    auto dst = mouseGrabber();
    if(dst)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        Point<int> leaf_offset = {0, 0};
        dst = leafAt(event_position, &leaf_offset);
        event_position -= leaf_offset;
    }

    if(dst->grabsMouseOnClick())
    {
        dst->ungrabMouse();
    }

    MouseReleaseEvent event(event_position, {0, 0}, button);
    dst->mouseReleaseEvent(&event);
}


void Widget::initMouseMoveEvent(Point<int> event_position)
{
    Point<int> event_delta = event_position - g_prev_mouse_position;
    g_prev_mouse_position = event_position;

    auto dst = Widget::mouseGrabber();
    if(dst)
    {
        event_position -= dst->toRootCoords(Point<int>(0, 0));
    }
    else
    {
        Point<int> leaf_offset = {0, 0};
        dst = leafAt(event_position, &leaf_offset);
        event_position -= leaf_offset;
    }

    MouseMoveEvent event(event_position, event_delta, pressed_buttons);
    dst->mouseMoveEvent(&event);
}