#include "Knob.h"
#include "MouseEvent.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{

Knob::Knob(Widget* parent) : Widget(parent)
{
    
}
    

inline float normalize360(float angle)
{
    if(angle < 0) while(angle < 0)
        angle+=360;
    else if(angle > 360) while(angle > 360)
        angle-=360;
    return angle;
}


inline float deg2rad(float angle)
{
    return angle * M_PI / 180;
}

    
inline float vertex_color(float angle, float light_angle)
{
    float min_angle = angle < light_angle ? angle : light_angle;
    float max_angle = angle > light_angle ? angle : light_angle;
    float angle_diff = max_angle - min_angle;
    if(angle_diff > M_PI)
        angle_diff = 2.0 * M_PI - angle_diff;
    
    return pow(angle_diff / M_PI, 3);
}
    
    
void Knob::render()
{
    glPushMatrix();
    glTranslatef(width() * 0.5, height() * 0.5, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    
    float light_angle = 270.0;
    float rotated_light_angle = normalize360(light_angle - angle);
    
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0, 0.0, 0.0);
        glVertex2f(0.0, 0.0);
        float ang = 0.0;
        for(int i=0; i<npoints; i++)
        {
            float color = vertex_color(ang, deg2rad(rotated_light_angle));
            glColor3f(color, color, color);
            glVertex2f(cos(ang) * radius, sin(ang) * radius);
            ang += _step;
        }
        float color = vertex_color(0.0, deg2rad(rotated_light_angle));
        glColor3f(color, color, color);
        glVertex2f(cos(0.0) * radius, sin(0.0) * radius);
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.3, 0.3, 0.3);
        glVertex2f(0.0, 0.0);
        ang = 0.0;
        for(int i=0; i<npoints; i++)
        {
            glColor3f(0.3, 0.3, 0.3);
            glVertex2f(cos(ang) * radius * 0.618, sin(ang) * radius * 0.618);
            ang += _step;
        }
        glColor3f(0.3, 0.3, 0.3);
        glVertex2f(cos(0.0) * radius * 0.618, sin(0.0) * radius);
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.5, 0.0, 0.0);
        glVertex2f(-radius * 0.1, radius * 0.3);
        glVertex2f( radius * 0.1, radius * 0.3);
        glColor3f(0.7, 0.0, 0.0);
        glVertex2f(-radius * 0.1, radius * 0.9);
        glVertex2f( radius * 0.1, radius * 0.9);
    glEnd();

    glPopMatrix();
}

    
void Knob::update()
{
    float width = radius * 2 + paddingLeft() + paddingRight();
    float height = radius * 2 + paddingBottom() + paddingTop();
    resize(width, height);
    _step = M_PI * 2.0 / npoints;
}


void Knob::mousePressEvent(MouseEvent* event)
{
    grabMouseInput();
    prev_mouse_y = event->position().y;
    event->has_been_handled = true;
}


void Knob::mouseReleaseEvent(MouseEvent* event)
{
    ungrabMouseInput();
    event->has_been_handled = true;
}


void Knob::mouseMoveEvent(MouseEvent* event)
{
    auto mouse_y = event->position().y;
    
    if(Widget::isMouseGrabber())
    {
        angle += (mouse_y - prev_mouse_y);
        if(angle < min_angle)
            angle = min_angle;
        else if(angle > max_angle)
            angle = max_angle;

        event->has_been_handled = true;
    }
    
    prev_mouse_y = mouse_y;
}

}//namespace r64fx