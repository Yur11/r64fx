#include "Window.h"
#include "Widget.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Painter.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
            
Window::VoidFun Window::event_callback = nullptr;    
vector<Window*> _all_window_instances;
Window* Window::currently_rendered_window = nullptr;
    

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
    currently_rendered_window = this;
    
    RenderingContext::makeCurrent();
    RenderingContext::update();
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(!one_shot_list.empty())
    {
        one_shot_list.exec();
        one_shot_list.clear();
    }
    
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    root_widget->render();
    currently_rendered_window = nullptr;
}


void Window::updateGeometry()
{
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    
    glViewport(0, 0, w, h);
    
    root_widget->setPosition(0.0, 0.0);
    root_widget->resize(w, h);
 
    int hw = (w >> 1);
    int hh = (h >> 1);
    
    float vec[4];
    vec[0] = 1.0 / hw;     //sx
    vec[1] = 1.0 / hh;     //sy
    vec[2] = -1.0;     //tx
    vec[3] = -1.0;     //ty
    
    Painter::setProjection(vec);
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
    
//     if(!_overlay_menus.empty())
//     {
//         Widget* menu = overlay_menu_at(x, y);
//         if(menu)
//         {
//             /* Deliver event to the menu. */
//             event -= menu->position();
//             menu->mousePressEvent(&event);
//             return;
//         }
//         else
//         {
//             /* Clicked elswere. */
//             closeAllOverlayMenus();
//         }
//     }
//     
//     
//     if(Widget::mouseInputGrabber())
//         Widget::mouseInputGrabber()->mousePressEvent(&event);
//     else
//         view()->mousePressEvent(&event);
}

    
void Window::initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
    
//     if(!_overlay_menus.empty())
//     {
//         Widget* menu = overlay_menu_at(x, y);
//         if(menu)
//         {
//             /* Event goes to the menu. */
//             event -= menu->position();
//             menu->mouseReleaseEvent(&event);
//             return;
//         }
//     }
//     else
//     {
//         if(Widget::mouseInputGrabber())
//             Widget::mouseInputGrabber()->mouseReleaseEvent(&event);
//         else
//             view()->mouseReleaseEvent(&event);
//     }
}

    
void Window::initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{    
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
 
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    root_widget->mouseMoveEvent(&event);
    
//     mouse_is_hovering_menu = false;
//     
//     static HoverableWidget* prev_hovered_widget = nullptr;
//     
//     if(!_overlay_menus.empty())
//     {
//         Widget* menu = overlay_menu_at(x, y);
//         if(menu)
//         {
//             /* Event goes to the menu. */
//             mouse_is_hovering_menu = true;
//             event -= menu->position();
//             menu->mouseMoveEvent(&event);
//             goto _exit;
//         }
//     }
//     
//     if(Widget::mouseInputGrabber())
//         Widget::mouseInputGrabber()->mouseMoveEvent(&event);
//     else
//     {
// #ifdef DEBUG
//         if(x >= view()->width())
//         {
//             cerr << "x == " << x << "\n";
//             cerr << "view->width() == " << view()->width() << "\n";
//             abort();
//         }
// #endif//DEBUG
//         view()->mouseMoveEvent(&event);
//     }
//     
// _exit:
//     if(event.hovered_widget == nullptr)
//     {
//         HoverableWidget::reset();
//         if(prev_hovered_widget != nullptr)
//         {
//             auto view = event.view();
//             if(view)
//                 view->getRepainted();
//         }
//     }
//  
//     
//     prev_hovered_widget = event.hovered_widget;
}


void Window::initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setButtons(event.buttons() | (dy > 0 ? Mouse::Button::WheelUp : Mouse::Button::WheelDown));
    
//     if(!_overlay_menus.empty())
//     {
//         Widget* menu = overlay_menu_at(x, y);
//         if(menu)
//         {
//             return;
//         }
//     }
//     
//     view()->mouseWheelEvent(&event);
}

    
void Window::initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
    event.setOriginWindow(this);
    
//     if(Widget::keyboardInputGrabber())
//     {
//         Widget::keyboardInputGrabber()->keyPressEvent(&event);
//     }
//     else
//     {
//         view()->keyPressEvent(&event);
//     }
}

    
void Window::initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
    event.setOriginWindow(this);
    
//     if(Widget::keyboardInputGrabber())
//     {
//         Widget::keyboardInputGrabber()->keyReleaseEvent(&event);
//     }
//     else
//     {
//         view()->keyReleaseEvent(&event);
//     }
}


void Window::initTextInputEvent(std::string text)
{
}


void Window::initResizeEvent(int w, int h)
{
    this->w = w;
    this->h = h;
    one_shot_list.push_back([](void* data){
        auto window = (Window*) data;
        window->updateGeometry();
    }, this);
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
        w->render();
    }
}

    
}//namespace r64fx
