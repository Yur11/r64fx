#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Menu.h"
#include "Window.h"
#include <iostream>
#include <assert.h>


using namespace std;

namespace r64fx{
    
/** Removeme */
extern Font* debug_font;


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
    
    const auto &r = rect();
    
    float data[16] = {
        r.left, r.bottom,
        r.right, r.bottom,
        r.left, r.top,
        r.right, r.top,
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
    };
    
    p.bindBuffer();
    p.setData(data);
    p.unbindBuffer();
}


void Dummy::render()
{  
    static float angle = 0.0;
    
//     Painter::enable();
    Painter::useCurrent2dProjection();
    Painter::setColor(0.0, 1.0, 0.0, 0.5);
    Painter::useNoTexture();
    
    float pos[2] = { float(width() + 10.0 * cos(angle)), float(height() + 10.0 * sin(angle)) };
    p.bindBuffer();
    p.setPositions(pos, 2, 6);
    p.unbindBuffer();
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
    
//     Painter::disable();
    
    angle += 0.1;
    if(angle >= M_PI*2.0) 
        angle = 0.0;
}


void Dummy::mousePressEvent(MouseEvent* event)
{
    if(event->buttons() & Mouse::Button::Right)
    {
        event->originWindow()->showOverlayMenu(event->original_x(), event->original_y(), _debug_menu);
    }
    else
    {
        auto p = toSceneCoords(event->position());
        cout << "dummy: " << event->x() << ", " << event->y() << " [" << p.x << ", " << p.y << "]\n";
    }
    
    event->has_been_handled = true;
}


void Dummy::mouseMoveEvent(MouseEvent* event)
{
    _last_mouse_move = event->position();
}


void Dummy::keyPressEvent(KeyEvent* event)
{
//     cout << "key: " << (int)event->ch() << "\n";
}


void Dummy::initDebugMenu()
{
    if(_debug_menu) return;
    
    _debug_menu = new Menu;
    _debug_menu->setPadding(5);
    _debug_menu->update();
}
    
}//namespace r64fx
