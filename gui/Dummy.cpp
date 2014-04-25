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
    auto br = boundingRect();
    
    auto pr = projectedRect();
    
    
//     cout << "br: " << br.left << ", " << br.top << ", " << br.right << ", " << br.bottom << "\n";
//     cout << "pr: " << pr.left << ", " << pr.top << ", " << pr.right << ", " << pr.bottom << "\n";
    
    float pos[8] = {
        pr.left,  pr.bottom,
        pr.right,  pr.bottom,
        pr.left,   pr.top,
        pr.right,  pr.top
    };
            
    p.bindBuffer();
    p.setPositions(pos, 8);
    p.unbindBuffer();
            
    Painter::setColor(1.0, 0.0, 0.0, 1.0);
    Painter::useNoTexture();
    
    p.bindArray();
    p.render(GL_LINE_LOOP, 4);
    p.unbindArray();
}


void Dummy::mousePressEvent(MouseEvent* event)
{    
    event->has_been_handled = true;
}


void Dummy::mouseMoveEvent(MouseEvent* event)
{
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
