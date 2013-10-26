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
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_TRIANGLE_FAN);
        step = M_PI / 3;
        angle = 0.0;
        glVertex2f(0.0, 0.0);
        for(int i=0; i<7; i++)
        {
            glVertex2f(nut_radius*cos(angle + M_PI * 0.5), nut_radius*sin(angle + M_PI * 0.5));
            angle += step;
        }
    glEnd();
    
    /* The socket */
    if(is_plugged)
    {
        glColor3f(0.2, 0.2, 0.2);
        glBegin(GL_TRIANGLE_FAN);
            step = M_PI / 16;
            angle = 0.0;
            glVertex2f(0.0, 0.0);
            for(int i=0; i<33; i++)
            {
                glVertex2f(0.9*hole_radius*cos(angle), 0.9*hole_radius*sin(angle));
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
                glVertex2f(0.6*hole_radius*cos(angle), 0.6*hole_radius*sin(angle));
                angle += step;
            }
        glEnd();
    }
    else
    {
        glColor3f(0.6, 0.6, 0.6);
        glBegin(GL_TRIANGLE_FAN);
            step = M_PI / 16;
            angle = 0.0;
            glVertex2f(0.0, 0.0);
            for(int i=0; i<33; i++)
            {
                glVertex2f(0.9*hole_radius*cos(angle), 0.9*hole_radius*sin(angle));
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
    }
    
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
    int i=count;
    while(i--)
        vertices.push_back(Point<float>(0.0, 0.0));
}



GLuint Wire::_texture = 0;

void render_wire_cap(Point<float> p, float angle, float wire_width)
{
    float a = angle;
    int nvertices = 8;
    float step = M_PI / nvertices;
    glBegin(GL_TRIANGLE_FAN);
        float ang = a;
        glTexCoord1f(0.5);
        glVertex2f(p.x, p.y);
        for(int i=0; i<=nvertices; i++)
        {
            glTexCoord1f(0.1);
            glVertex2f(
                p.x + (float)cos(ang) * wire_width,
                p.y + (float)sin(ang) * wire_width
            );
            ang+=step;
        }
    glEnd();
}

void Wire::render()
{    
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, _texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glColor(color);
    
    render_wire_cap(vertices.front().position, vertices.front().perpendicular_angle, Wire::width);
    render_wire_cap(vertices.back().position, vertices.back().perpendicular_angle + M_PI, Wire::width);
    
    glBegin(GL_TRIANGLE_STRIP);
        for(int i=0; i<(int)vertices.size(); i++)
        {
            auto &v = vertices[i];

            glTexCoord1f(0.1);
            glVertex2f(
                v.perp_a.x,
                v.perp_a.y
            );
            
            glTexCoord1f(0.9);
            glVertex2f(
                v.perp_b.x,
                v.perp_b.y
            );
        }
    glEnd();
    
    glDisable(GL_TEXTURE_1D);
}


void gen_wire_vertex_pair(Wire::Vertex &v, float wire_width)
{
    v.perp_a = {
        v.position.x + (float)cos(v.perpendicular_angle) * wire_width,
        v.position.y + (float)sin(v.perpendicular_angle) * wire_width
    };
    
    v.perp_b = {
        v.position.x - (float)cos(v.perpendicular_angle) * wire_width,
        v.position.y - (float)sin(v.perpendicular_angle) * wire_width
    };
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
    auto bottommost = pa.y < pb.y ? pa.y : pb.y;
    Point<float> pc(leftmost + (rightmost - leftmost) * 0.5, bottommost - 500.0);
    
    points = { pa, pc, pb };
    
    bezier_points(points, vertices);
    
    /* Calculate tangents. */
    for(int i=1; i<(int)vertices.size()-1; i++)
    {
        auto &v = vertices[i];
        auto &v1 = vertices[i-1];
        auto &v2 = vertices[i+1];
        auto dy = v2.position.y - v1.position.y;
        auto dx = v2.position.x - v1.position.x;
        
        float angle;
        
        if(dx == 0)
            angle = 0.0;
        else
            angle = atan2(dy, dx);
        
        angle += M_PI * 0.5;
        
        if(angle > M_PI * 2)
            angle -= M_PI * 2;

        v.perpendicular_angle = angle;
        gen_wire_vertex_pair(v, Wire::width);
    } 
    
    vertices[0].perpendicular_angle = vertices[1].perpendicular_angle;
    gen_wire_vertex_pair(vertices[0], Wire::width);
    
    vertices[vertices.size()-1].perpendicular_angle = vertices[vertices.size()-2].perpendicular_angle;
    gen_wire_vertex_pair(vertices.back(), Wire::width);
}


void Wire::init()
{
    /*Make a nice round wire texture.*/
    
    const int tex_size = 32;
  
    unsigned char bytes[tex_size * 4];
    
    float step = 1.0 / tex_size;
    float f = 0;
    
    for(int i=0; i<tex_size * 4; i+=4)
    {
        unsigned char b = 255 * sin(f*M_PI);
        bytes[i + 0] = b;
        bytes[i + 1] = b;
        bytes[i + 2] = b;
        bytes[i + 3] = 0;
        f += step;
    }
    
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_1D, _texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    gluBuild1DMipmaps(GL_TEXTURE_1D, GL_RGBA, tex_size, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
}
    
}//namespace r64fx
