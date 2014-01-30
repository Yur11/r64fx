#include "Window.h"
#include "KeyEvent.h"
#include <iostream>

using namespace std;

namespace r64fx{
            
vector<Window*> _all_window_instances;
    
Window::Window()
{
    _all_window_instances.push_back(this);
}


Window::~Window()
{
    for(int i=0; i<(int)_all_window_instances.size(); i++)
    {
        if(_all_window_instances[i] == this)
        {
            _all_window_instances.erase(_all_window_instances.begin() + i);
            break;
        }
    }
}



void Window::render()
{
    _view->render();
    
    render_overlay_menus();
}


void Window::render_overlay_menus()
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


Widget* Window::overlay_menu_at(int x, int y)
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


void Window::showOverlayMenu(int x, int y, Menu* menu)
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

    
void Window::closeOverlayMenu(Menu* menu)
{
    while(_overlay_menus.back() != menu)
    {
        _overlay_menus.pop_back();
    }
}


void Window::closeAllOverlayMenus()
{
    _overlay_menus.clear();
}


std::vector<Window*> Window::allInstances()
{
    return _all_window_instances;
}


void Window::initMousePressEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
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
    
    
    if(Widget::mouseInputGrabber())
        Widget::mouseInputGrabber()->mousePressEvent(&event);
    else
        view()->mousePressEvent(&event);
}

    
void Window::initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
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
        if(Widget::mouseInputGrabber())
            Widget::mouseInputGrabber()->mouseReleaseEvent(&event);
        else
            view()->mouseReleaseEvent(&event);
    }
}

    
void Window::initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
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
    
    if(Widget::mouseInputGrabber())
        Widget::mouseInputGrabber()->mouseMoveEvent(&event);
    else
        view()->mouseMoveEvent(&event);
}


void Window::initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setButtons(event.buttons() | (dy > 0 ? Mouse::Button::WheelUp : Mouse::Button::WheelDown));
    
    if(!_overlay_menus.empty())
    {
        Widget* menu = overlay_menu_at(x, y);
        if(menu)
        {
            return;
        }
    }
    
    view()->mouseWheelEvent(&event);
}

    
void Window::initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
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

    
void Window::initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
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


void Window::initTextInputEvent(Utf8String text)
{
}


bool Window::initGlew()
{
    auto status = glewInit();
    if(status != GLEW_OK)
    {
        cerr << "Failed to init GLEW!\n";
        return false;
    }
    
    if(!GLEW_VERSION_3_0)
    {
        cerr << "OpenGL 3.0 not supported!\n";
        return false;
    }
    
    return true;
}

    
}//namespace r64fx
