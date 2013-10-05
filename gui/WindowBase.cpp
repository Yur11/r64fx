#include "WindowBase.h"
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


void WindowBase::renderAll()
{
#ifdef DEBUG
    if(!view())
    {
        cerr << "View is null!\n";
        abort();
    }
#endif//DEBUG

    view()->render();
    
    render_overlay_menus();
    
    glFinish();
}


void WindowBase::cacheFrambuffer()
{
    glBindTexture(GL_TEXTURE_2D, _cache_texture);
    glCopyTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        0, 0, max_width, max_height,
        0
    );
}


void WindowBase::renderCached()
{
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(float(width()) / float(max_width), 0.0);
        glVertex2f(width(), 0.0);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f(float(width()) / float(max_width), float(height()) / float(max_height));
        
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, float(height()) / float(max_height));
    glEnd();
    
    glFinish();
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


void WindowBase::updateCacheTexture()
{
    if(_cache_texture != 0)
    {
        glDeleteTextures(1, &_cache_texture);
    }
    
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &_cache_texture);
    glBindTexture(GL_TEXTURE_2D, _cache_texture);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    GLfloat flargest;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &flargest);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, flargest);
    
    unsigned char* bytes = new unsigned char[max_width * max_height * 4];
    for(int i=0; i<max_width * max_height * 4; i++)
    {
        bytes[i] = 30;
    }
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max_width, max_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, max_width, max_height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    delete[] bytes;
    
    glDisable(GL_TEXTURE_2D);
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


void WindowBase::initMousePressEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
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

    
void WindowBase::initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
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

    
void WindowBase::initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
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


void WindowBase::initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers)
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

    
void WindowBase::initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
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

    
void WindowBase::initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
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


void WindowBase::initTextInputEvent(Utf8String text)
{
}

    
}//namespace r64fx
