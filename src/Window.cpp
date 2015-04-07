#include "Window.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "TextInputEvent.hpp"

#include <iostream>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
            
Window::VoidFun Window::event_callback = nullptr;    
vector<Window*> all_window_instances;
Window* Window::currently_rendered_window = nullptr;
    

Window::Window()
{
    all_window_instances.push_back(this);
}


Window::~Window()
{
}


std::vector<Window*> Window::allInstances()
{
    return all_window_instances;
}


bool Window::isSDL2()
{
    return false;
}


void Window::initMousePressEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
    
//     if(Widget::mouseInputGrabber())
//     {
//         Widget::mouseInputGrabber()->mousePressEvent(&event);
//     }
//     
// #ifdef DEBUG
//     assert(root_widget != nullptr);
// #endif//DEBUG
//     root_widget->mousePressEvent(&event);
}

    
void Window::initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
    
//     if(Widget::mouseInputGrabber())
//     {
//         Widget::mouseInputGrabber()->mouseReleaseEvent(&event);
//     }
//     
// #ifdef DEBUG
//     assert(root_widget != nullptr);
// #endif//DEBUG
//     root_widget->mouseReleaseEvent(&event);
}

    
void Window::initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{    
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
 
//     if(Widget::mouseInputGrabber())
//     {
//         Widget::mouseInputGrabber()->mouseMoveEvent(&event);
//     }
//     
// #ifdef DEBUG
//     assert(root_widget != nullptr);
// #endif//DEBUG
//     root_widget->mouseMoveEvent(&event);
}


void Window::initMouseWheelEvent(int x, int y, int dx, int dy, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setButtons(event.buttons() | (dy > 0 ? Mouse::Button::WheelUp : Mouse::Button::WheelDown));
}

    
void Window::initKeyPressEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
    event.setOriginWindow(this);
}

    
void Window::initKeyReleaseEvent(int x, int y, unsigned int scancode, unsigned int buttons, unsigned int keyboard_modifiers)
{
    KeyEvent event(x, y, buttons, scancode, keyboard_modifiers);
    event.setOriginWindow(this);
}


void Window::initTextInputEvent(std::string text)
{
    TextInputEvent event(text);
    event.setOriginWindow(this);
//     if(Widget::keyboardInputGrabber())
//         Widget::keyboardInputGrabber()->textInputEvent(&event);
//     else
//         root_widget->textInputEvent(&event);
}


void Window::initResizeEvent(int w, int h)
{
    this->w = w;
    this->h = h;
//     one_shot_list.push_back([](void* data){
//         auto window = (Window*) data;
//         window->updateGeometry();
//         window->render();
//     }, this);
}


void Window::mainSequence()
{
    event_callback();
/*    
    for(auto w : all_window_instances)
    {
        w->makeCurrent();
        w->update();
        w->runOneShotList();
        w->render();
    }*/
}

    
}//namespace r64fx
