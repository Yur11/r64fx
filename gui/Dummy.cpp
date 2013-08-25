#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Menu.h"
#include "WindowBase.h"
#include <iostream>


using namespace std;

namespace r64fx{


void* on_menu_click(void* source, void* data)
{
    auto act = (Action*) source;
    cout << act->name().stdstr << "\n";
    
    return nullptr;
}
    

Menu* _debug_menu = nullptr;
    
void Dummy::render()
{    
    glColor3f(1.0, 0.0, 0.0);
    
    render_bounding_rect();
    
    glPointSize(5.0);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    glVertex2f(_last_mouse_move.x, _last_mouse_move.y);
    glEnd();
    glPointSize(1.0);
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
    _debug_menu->appendAction(new Action(Icon::find("D1", 18, 18), "Hello",     Message(on_menu_click)));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Doctor",    Message(on_menu_click)));
    _debug_menu->appendAction(new Action(/*Icon::find("D1", 32, 32),*/ "Name",      Message(on_menu_click)));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Continue",  Message(on_menu_click)));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Yesterday", Message(on_menu_click)));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Tommorow",  Message(on_menu_click)));
    _debug_menu->setPadding(5);
    _debug_menu->setPaddingTop(10);
    _debug_menu->setSpacing(10);
    _debug_menu->update();
}
    
}//namespace r64fx
