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
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    
    root_widget->setRelativePosition(0.0, 0.0);
    root_widget->resize(w, h);
    root_widget->update();
 
    int hw = (w >> 1);
    int hh = (h >> 1);
    
    float vec[4];
    vec[0] = 1.0 / hw;    //sx
    vec[1] = -(1.0 / hh); //sy flipped around x axis
    vec[2] = -1.0;        //tx
    vec[3] =  1.0;        //ty
    
    Painter::setProjection(vec);
}


void Window::runOneShotList()
{
    one_shot_list.exec();
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
    
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    root_widget->mousePressEvent(&event);
}

    
void Window::initMouseReleaseEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
    
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    root_widget->mouseReleaseEvent(&event);
}

    
void Window::initMouseMoveEvent(int x, int y, unsigned int buttons, unsigned int keyboard_modifiers)
{    
    MouseEvent event(x, y, buttons, keyboard_modifiers);
    event.setOriginWindow(this);
 
#ifdef DEBUG
    assert(root_widget != nullptr);
#endif//DEBUG
    root_widget->mouseMoveEvent(&event);
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
}


void Window::initResizeEvent(int w, int h)
{
    this->w = w;
    this->h = h;
    one_shot_list.push_back([](void* data){
        auto window = (Window*) data;
        window->updateGeometry();
        window->render();
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
        w->makeCurrent();
        w->update();
        w->runOneShotList();
        w->render();
    }
}


void Window::issueRepaint()
{
//     cout << "issue_repaint\n";
//     fireOnce([](void* data){
//         auto window = (Window*) data;
//         cout << "repaint\n";
//         window->render();
//     }, this);
}

    
}//namespace r64fx
