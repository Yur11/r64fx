#include "WindowBase.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include <iostream>

using namespace std;

namespace r64fx{
            
vector<WindowBase*> _all_window_base_instances;
    
WindowBase::WindowBase()
{
    _all_window_base_instances.push_back(this);
}


WindowBase::~WindowBase()
{
    for(int i=0; i<(int)_all_window_base_instances.size(); i++)
    {
        if(_all_window_base_instances[i] == this)
        {
            _all_window_base_instances.erase(_all_window_base_instances.begin() + i);
            break;
        }
    }
}


void WindowBase::render_overlay_menus()
{
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(auto &menu : _overlay_menus)
    {
        glPushMatrix();
        glTranslatef(menu->x(), menu->y(), 0.0);
        menu->render();
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
}


Widget* WindowBase::overlay_menu_at(int x, int y)
{
    for(auto &menu : _overlay_menus)
    {
        if(menu->rect().overlaps(x, y))
        {
            return menu;
        }
    }
    
    return nullptr;
}


void WindowBase::showOverlayMenu(int x, int y, Menu* menu)
{
    if(y > height() / 2)
    {
        y -= menu->height();
    }
    
    if(x > width()/ 2)
    {
        x -= menu->width();
    }
    
    menu->setPosition(x, y);
    _overlay_menus.push_back(menu);
}

    
void WindowBase::closeOverlayMenu(Menu* menu)
{
    while(_overlay_menus.back() != menu)
    {
        _overlay_menus.pop_back();
    }
}


void WindowBase::closeAllOverlayMenus()
{
    _overlay_menus.clear();
}


std::vector<WindowBase*> WindowBase::allInstances()
{
    return _all_window_base_instances;
}


void WindowBase::initMousePressEvent(int x, int y, unsigned int buttons)
{
    MouseEvent event(x, y, buttons);
    _last_mouse_event = event;
    event.setOriginWindow(this);
    
    if(!_overlay_menus.empty())
    {
        Widget* menu = overlay_menu_at(x, y);
        if(menu)
        {
            /* Deliver event to the menu. */
            event -= menu->position();
            menu->mousePressEvent(&event);
            return;
        }
        else
        {
            /* Clicked elswere. */
            closeAllOverlayMenus();
        }
    }
    
    view()->mousePressEvent(&event);
}

    
void WindowBase::initMouseReleaseEvent(int x, int y, unsigned int buttons)
{
    MouseEvent event(x, y, buttons);
    _last_mouse_event = event;
    event.setOriginWindow(this);
    
    if(!_overlay_menus.empty())
    {
        Widget* menu = overlay_menu_at(x, y);
        if(menu)
        {
            /* Event goes to the menu. */
            event -= menu->position();
            menu->mouseReleaseEvent(&event);
            return;
        }
    }
    else
    {
        view()->mouseReleaseEvent(&event);
    }
}

    
void WindowBase::initMouseMoveEvent(int x, int y, unsigned int buttons)
{
    MouseEvent event(x, y, buttons);
    _last_mouse_event = event;
    event.setOriginWindow(this);
    
    if(!_overlay_menus.empty())
    {
        Widget* menu = overlay_menu_at(x, y);
        if(menu)
        {
            /* Event goes to the menu. */
            event -= menu->position();
            menu->mouseMoveEvent(&event);
            return;
        }
    }
    else
    {
        view()->mouseMoveEvent(&event);
    }
}


void WindowBase::initMouseWheelEvent(int dx, int dy, unsigned int buttons)
{
    MouseEvent event = _last_mouse_event;
    event.setButtons(event.buttons() | (dy > 0 ? Mouse::Button::WheelUp : Mouse::Button::WheelDown));
    view()->mouseWheelEvent(&event);
}

    
void WindowBase::initKeyPressEvent(unsigned int scancode, unsigned int buttons, unsigned int modifiers)
{
    KeyEvent event(scancode, buttons, modifiers, &_last_mouse_event);
    event.setOriginWindow(this);
    
    if(Widget::keyboardInputGrabber())
    {
        Widget::keyboardInputGrabber()->keyPressEvent(&event);
    }
    else
    {
        view()->keyPressEvent(&event);
    }
}

    
void WindowBase::initKeyReleaseEvent(unsigned int scancode, unsigned int buttons,  unsigned int modifiers)
{
    KeyEvent event(scancode, buttons, modifiers, &_last_mouse_event);
    event.setOriginWindow(this);
    
    if(Widget::keyboardInputGrabber())
    {
        Widget::keyboardInputGrabber()->keyReleaseEvent(&event);
    }
    else
    {
        view()->keyReleaseEvent(&event);
    }
}


void WindowBase::initTextInputEvent(Utf8String text)
{
}

    
}//namespace r64fx
