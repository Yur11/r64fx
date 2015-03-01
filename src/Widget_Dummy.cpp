#include "Dummy.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "Menu.hpp"
#include "Window.hpp"
#include <iostream>
#include <assert.h>


using namespace std;

namespace r64fx{
    
void* on_menu_click(void* source, void* data)
{
    auto act = (Action*) source;
    cout << act->name << "\n";
    
    return nullptr;
}
    

Menu* _debug_menu = nullptr;
    

Dummy::Dummy(float width, float height, Widget* parent) 
: Widget(parent) 
, p(4)
{
    setSize(width, height);
    
    float tex_coords[8] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p.bindBuffer();
    p.setTexCoords(tex_coords, 8);
    p.unbindBuffer();
}


void Dummy::render()
{          
    auto r = projectedRect();
    
    float pos[8] = {
        int(r.left)  + 0.5f,  int(r.bottom) + 0.5f,
        int(r.right) + 0.5f,  int(r.bottom) + 0.5f,
        int(r.left)  + 0.5f,  int(r.top)    + 0.5f,
        int(r.right) + 0.5f,  int(r.top)    + 0.5f
    };
            
    p.bindBuffer();
    p.setPositions(pos, 8);
    p.unbindBuffer();
         
    if(is_pressed)
        Painter::setColor(0.0, 1.0, 0.0, 1.0);
    else
        Painter::setColor(1.0, 0.0, 0.0, 1.0);
    Painter::useNoTexture();
    
    p.bindArray();
    p.render(GL_LINE_LOOP, 4);
    p.unbindArray();
}


void Dummy::mousePressEvent(MouseEvent* event)
{    
    event->has_been_handled = true;
    is_pressed = true;
    grabMouseInput();
    event->originWindow()->issueRepaint();
}


void Dummy::mouseReleaseEvent(MouseEvent* event)
{
    event->has_been_handled = true;
    is_pressed = false;
    if(isMouseInputGrabber())
        ungrabMouseInput();
    event->originWindow()->issueRepaint();
}


void Dummy::mouseMoveEvent(MouseEvent* event)
{
    event->has_been_handled = true;
    _last_mouse_move = event->position();
}


void Dummy::keyPressEvent(KeyEvent* event)
{
}


void Dummy::initDebugMenu()
{
    if(_debug_menu) return;
    
    _debug_menu = new Menu;
    _debug_menu->update();
}
    
}//namespace r64fx
