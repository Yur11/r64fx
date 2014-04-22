#include "Knob.h"
#include "gui/MouseEvent.h"
#include "gui/bezier.h"
#include "gui/Window.h"
#include "gui/Painter.h"
#include "gui/geometry_io.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
extern string data_prefix;


BasicKnob::BasicKnob(Widget* parent) 
: Widget(parent)
{
    
}

    
void BasicKnob::update()
{
    float width = radius * 2 + paddingLeft() + paddingRight();
    float height = radius * 2 + paddingBottom() + paddingTop();
    resize(width, height);
}


void BasicKnob::mousePressEvent(MouseEvent* event)
{
    grabMouseInput();
    Mouse::setRelativeMode(true);
    Mouse::relativePosition();//Kluge!
    event->has_been_handled = true;
}


void BasicKnob::mouseReleaseEvent(MouseEvent* event)
{
    ungrabMouseInput();
    Mouse::setRelativeMode(false);
    event->has_been_handled = true;
}


void BasicKnob::mouseMoveEvent(MouseEvent* event)
{    
    if(Widget::isMouseGrabber())
    {
        angle -= Mouse::relativePosition().y;
        if(angle < min_angle)
            angle = min_angle;
        else if(angle > max_angle)
            angle = max_angle;

        update();
//         value_changed.send(this);
        
        event->has_been_handled = true;
    }
}


ShinyKnob::ShinyKnob(Texture2D* bg, Texture2D* fg, Texture2D* shiny)
: bg(bg)
, fg(fg)
, shiny(shiny)
, p(12)
{
    float data[24] = {
        0.0, 1.0,
        1.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p.bindBuffer();
    p.setTexCoords(data, 24);
    p.unbindBuffer();
}


void ShinyKnob::update()
{
    BasicKnob::update();
    
    auto c = Point<float>( width() * 0.5, height() * 0.5 );
    
    float r = radius * 0.6;
    
    Point<float> pt[4] = {
        { -r, -r },
        {  r, -r },
        { -r,  r },
        {  r,  r }
    };
    
    for(int i=0; i<4; i++)
    {
        pt[i].rotate( - angle * M_PI / 180);
    }

    r *= 0.7;

    float data[24] = {
        /* bg */
        0.0, 0.0,
        width(), 0.0,
        0.0, height(),
        width(), height(),
        
        /* rotated fg */
        pt[0].x + c.x, pt[0].y + c.y,
        pt[1].x + c.x, pt[1].y + c.y,
        pt[2].x + c.x, pt[2].y + c.y,
        pt[3].x + c.x, pt[3].y + c.y,
        
        /* shiny fg */
        -r + c.x, -r + c.y,
         r + c.x, -r + c.y,
        -r + c.x,  r + c.y,
         r + c.x,  r + c.y
    };
    
    p.bindBuffer();
    p.setPositions(data, 24);
    p.unbindBuffer();
}


void ShinyKnob::render()
{
    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
//     Painter::useCurrent2dProjection();
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    
    p.bindArray();
    
    Painter::setTexture(bg->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 0);
    
    Painter::setTexture(fg->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 4);
    
    Painter::setTexture(shiny->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 8);
    
    p.unbindArray();
    
    gl::Disable(GL_BLEND);
}


}//namespace r64fx