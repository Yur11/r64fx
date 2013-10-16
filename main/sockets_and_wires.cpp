#include "sockets_and_wires.h"
#include "bezier.h"
#include "MouseEvent.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    

Socket::Socket(Widget* parent) : Widget(parent)
{
}


void Socket::render()
{
    float angle;
    float step;
    
    glPushMatrix();
    glTranslated(width() * 0.5, height() * 0.5, 0.0);
    
    /* The nut */
    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_TRIANGLE_FAN);
        step = M_PI / 16;
        angle = 0.0;
        glVertex2f(0.0, 0.0);
        for(int i=0; i<33; i++)
        {
            glVertex2f(nut_radius*cos(angle), nut_radius*sin(angle));
            angle += step;
        }
    glEnd();
    
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_TRIANGLE_FAN);
        step = M_PI / 3;
        angle = 0.0;
        glVertex2f(0.0, 0.0);
        for(int i=0; i<7; i++)
        {
            glVertex2f(nut_radius*cos(angle), nut_radius*sin(angle));
            angle += step;
        }
    glEnd();
    
    /* The socket */
    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_TRIANGLE_FAN);
        step = M_PI / 16;
        angle = 0.0;
        glVertex2f(0.0, 0.0);
        for(int i=0; i<33; i++)
        {
            glVertex2f(hole_radius*cos(angle), hole_radius*sin(angle));
            angle += step;
        }
    glEnd();
    
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_TRIANGLE_FAN);
        step = M_PI / 16;
        angle = 0.0;
        glVertex2f(0.0, 0.0);
        for(int i=0; i<33; i++)
        {
            glVertex2f(0.7*hole_radius*cos(angle), 0.7*hole_radius*sin(angle));
            angle += step;
        }
    glEnd();
    
    glPopMatrix();
}


void Socket::update()
{
    setWidth(nut_radius * 2.0);
    setHeight(nut_radius * 2.0);
}


void Socket::mousePressEvent(MouseEvent* event)
{
    auto tsc = toSceneCoords(event->position());
    cout << "( socket: " << event->position().x << ", " << event->position().y << " ) " << tsc.x << ", " << tsc.y << "\n";
    
    if(!wire)
    {
        mouse_pressed = false;
        event->has_been_handled = true;
    }
}


void Socket::mouseMoveEvent(MouseEvent* event)
{
    if(mouse_pressed)
    {
        auto wire = new Wire();
        wire->setA(this);
        drag_started.send(this);
        event->has_been_handled = true;
    }
}


Wire::~Wire()
{
    if(a)
        a->wire = nullptr;
    if(b)
        b->wire = nullptr;
}


void Wire::setVertexCount(int count)
{
    vertices.clear();
    int i=32;
    while(i--)
        vertices.push_back(Point<float>(0.0, 0.0));
}


void Wire::update()
{
#ifdef DEBUG
    assert(a != nullptr);
    assert(b != nullptr);
#endif//DEBUG
    
    Point<float> pa = a->toSceneCoords(a->size().toPoint() * 0.5);
    Point<float> pb = b->toSceneCoords(b->size().toPoint() * 0.5);
    
    auto leftmost = pa.x < pb.x ? pa.x : pb.x;
    auto rightmost = pa.x > pb.x ? pa.x : pb.x;
    auto topmost = pa.y > pb.y ? pa.y : pb.y;
    auto bottommost = pa.y < pb.y ? pa.y : pb.y;
    Point<float> pc(leftmost + (rightmost - leftmost) * 0.5, bottommost - 200.0);
    
    points = { pa, pc, pb };
    
    bezier_points(points, vertices);
    
    for(auto v : vertices)
    {
//         cout << v.x << ", " << v.y << "\n";
    }
}
    
}//namespace r64fx