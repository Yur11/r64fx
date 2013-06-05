#include "Dummy.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include <iostream>


using namespace std;

namespace r64fx{
    
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
    auto p = toSceneCoords(event->position());
    cout << "dummy: " << event->x() << ", " << event->y() << " [" << p.x << ", " << p.y << "]\n";
}


void Dummy::mouseMoveEvent(MouseEvent* event)
{
    _last_mouse_move = event->position();
}


void Dummy::keyPressEvent(KeyEvent* event)
{
//     cout << "key: " << (int)event->ch() << "\n";
}
    
}//namespace r64fx
