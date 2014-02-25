#include "Window.h"
#include "Error.h"
#include "KeyEvent.h"
#include <iostream>

using namespace std;

namespace r64fx{
            
Window::VoidFun Window::event_callback = nullptr;
    
vector<Window*> _all_window_instances;
    
Window::Window(RenderingContextId_t id)
: RenderingContext(id)
{
    _all_window_instances.push_back(this);
}


Window::~Window()
{
}


void Window::render()
{
    _view->render();
    render_overlay_menus();
}


void Window::update_viewport()
{
    glViewport(0, 0, new_w, new_h);
}


void Window::update_projection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /* (0, 0) is in the bottom left corner. */
    glOrtho(0, new_w, 0, new_h, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    view()->resize(0, new_h, new_w, 0);
    
    *current_2d_projection = Projection2D::ortho2d(0, new_w, 0, new_h);
}


void Window::render_overlay_menus()
{
    glDisable(GL_SCISSOR_TEST);
    CHECK_FOR_GL_ERRORS;
    glEnable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_FOR_GL_ERRORS;
    for(auto &menu : _overlay_menus)
    {
//         glPushMatrix();
        auto p = *current_2d_projection;
        
//         glTranslatef(menu->x(), menu->y(), 0.0);
        current_2d_projection->translate(menu->x(), menu->y());
        
        menu->render();
        
//         glPopMatrix();
        *current_2d_projection = p;
    }
    glDisable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glEnable(GL_SCISSOR_TEST);
    CHECK_FOR_GL_ERRORS;
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


void Window::discard()
{
    /* Remove this window from the main main sequence. */
    for(int i=0; i<(int)_all_window_instances.size(); i++)
    {
        if(_all_window_instances[i] == this)
        {
            _all_window_instances.erase(_all_window_instances.begin() + i);
            break;
        }
    }
    
    deleteLater();
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


void Window::mainSequence()
{
    event_callback();
    
    for(auto w : _all_window_instances)
    {
        w->makeCurrent();
        
        if(w->viewport_update_needed)
        {
            w->update_viewport();
            w->update_projection();
            w->viewport_update_needed = false;
        }
        
        w->update();
        w->render();
    }
}

    
}//namespace r64fx
