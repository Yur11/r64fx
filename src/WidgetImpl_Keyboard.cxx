/* To be included in Widget.cpp */

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
