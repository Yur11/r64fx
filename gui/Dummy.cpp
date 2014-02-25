#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Menu.h"
#include "Window.h"
#include "RectVertices.h"
#include "Error.h"
#include <iostream>
#include <assert.h>


using namespace std;

namespace r64fx{
    
/** Removeme */
extern Font* debug_font;


void* on_menu_click(void* source, void* data)
{
    auto act = (Action*) source;
    cout << act->name().stdstr << "\n";
    
    return nullptr;
}
    

Menu* _debug_menu = nullptr;
    
void Dummy::render()
{  
    auto rv = RectVertices::instance();
    rv->setRect(rect());
        
    Painter::enable();
    Painter::setColor({1.0, 1.0, 1.0, 1.0});
    Painter::useCurrent2dProjection();
    rv->bindArray();
    Painter::paint(GL_TRIANGLE_STRIP, rv->size());
    rv->unbindArray();
    Painter::disable();
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
    
    _debug_menu = new Menu(debug_font);
    _debug_menu->setPadding(5);
    _debug_menu->update();
}
    
}//namespace r64fx
