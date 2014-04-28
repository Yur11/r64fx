#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Menu.h"
#include "Window.h"
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
    resize(width, height);
    
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
    auto r = absoluteRect();
    
    float pos[8] = {
        r.left,   r.bottom,
        r.right,  r.bottom,
        r.left,   r.top,
        r.right,  r.top
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
    event->originWindow()->issueRepaint();
}


void Dummy::mouseReleaseEvent(MouseEvent* event)
{
    event->has_been_handled = true;
    is_pressed = false;
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
