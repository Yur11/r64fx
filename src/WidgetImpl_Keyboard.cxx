/* To be included in Widget.cpp */

namespace r64fx{

/* Widget that currently has keyboard focus. */
Widget* g_focus_owner = nullptr;


bool Widget::getsFocusOnClick(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_CLICK_FOCUS;
    else
        m_flags &= ~R64FX_WIDGET_CLICK_FOCUS;
    return yes;
}


bool Widget::getsFocusOnClick() const
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


void Widget::initKeyPressEvent(unsigned int key)
{
    Keyboard::trackModifierPress(key);

    KeyPressEvent event(key);
    if(g_focus_owner)
    {
        g_focus_owner->keyPressEvent(&event);
    }
    else
    {
        keyPressEvent(&event);
    }
}


void Widget::initKeyReleaseEvent(unsigned int key)
{
    Keyboard::trackModifierRelease(key);

    KeyReleaseEvent event(key);
    if(g_focus_owner)
    {
        g_focus_owner->keyReleaseEvent(&event);
    }
    else
    {
        keyReleaseEvent(&event);
    }
}


void Widget::initTextInputEvent(const std::string &text, unsigned int key)
{
    Keyboard::trackModifierPress(key);

    TextInputEvent event(text, key);
    if(g_focus_owner)
    {
        g_focus_owner->textInputEvent(&event);
    }
    else
    {
        textInputEvent(&event);
    }
}

}//namespace r64fx
