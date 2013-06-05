#include "WindowBase.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
        
WindowBase::WindowBase()
{
   
}

void WindowBase::debug_init(ContextMenu* menu)
{
    this->menu = menu;
}


void WindowBase::render_overlays()
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


void WindowBase::showOverlayMenu(int x, int y, VerticalMenu* menu)
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

    
void WindowBase::closeOverlayMenu(VerticalMenu* menu)
{
    
}


void WindowBase::closeAllOverlayMenus()
{
    _overlay_menus.clear();
}


void WindowBase::initMousePressEvent(int x, int y, unsigned int buttons)
{
    MouseEvent event(x, y, buttons);
    event.window = this;
    
    if(!_overlay_menus.empty())
    {
        Widget* menu = overlay_menu_at(x, y);
        if(menu)
        {
            /* Event goes to the menu. */
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

    /* Temporary. Menus shall be requested by objects int the scene. */
    if(buttons & Mouse::Button::Right)
    {
        showOverlayMenu(x, y, menu);
    }
    
    view()->mousePressEvent(&event);
}

    
void WindowBase::initMouseReleaseEvent(int x, int y, unsigned int buttons)
{
    MouseEvent event(x, y, buttons);
    event.window = this;
    
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
    event.window = this;
    
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
    MouseEvent event;
}

    
void WindowBase::initKeyPressEvent(unsigned int scancode, unsigned int buttons)
{
    KeyEvent event(scancode, buttons);
}

    
void WindowBase::initKeyReleaseEvent(unsigned int scancode, unsigned int buttons)
{
    KeyEvent event(scancode, buttons);
}


void WindowBase::initTextInputEvent(Utf8String text)
{
}

    
}//namespace r64fx
