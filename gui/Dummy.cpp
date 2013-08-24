#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Menu.h"
#include "WindowBase.h"
#include <iostream>


using namespace std;

namespace r64fx{


Menu* _debug_menu = nullptr;
    
void Dummy::render()
{    
    glColor3f(1.0, 0.0, 0.0);
    
    render_bounding_rect();
    
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    Texture::defaultTexture().bind();
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, height());
        glTexCoord2f(1.0, 1.0);
        glVertex2f(width(), height());
        glTexCoord2f(1.0, 0.0);
        glVertex2f(width(), 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
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
    _debug_menu->appendAction(new Action(Icon::find("D1", 18, 18), "Hello"));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Doctor"));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Name"));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Continue"));
    _debug_menu->appendAction(new Action(Icon::find("D1", 32, 32), "Yesterday"));
    _debug_menu->appendAction(new Action("Tommorow"));
    _debug_menu->setPadding(5);
    _debug_menu->setSpacing(10);
    _debug_menu->update();
}
    
}//namespace r64fx
