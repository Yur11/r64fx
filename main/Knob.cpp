#include "Knob.h"
#include "gui/Error.h"
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
        value_changed.send(this);
        
        event->has_been_handled = true;
    }
}


ShinyKnob::ShinyKnob(Texture bg, Texture fg, Texture shiny)
: bg(bg)
, fg(fg)
, shiny(shiny)
, pv(12)
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
    
    pv.bindBuffer();
    pv.setTexCoords(data, 24);
    pv.unbindBuffer();
}


void ShinyKnob::update()
{
    BasicKnob::update();
    
    auto c = Point<float>( width() * 0.5, height() * 0.5 );
    
    float r = radius * 0.6;
    
    Point<float> p[4] = {
        { -r, -r },
        {  r, -r },
        { -r,  r },
        {  r,  r }
    };
    
    for(int i=0; i<4; i++)
    {
        p[i].rotate( - angle * M_PI / 180);
    }

    r *= 0.7;

    float data[24] = {
        /* bg */
        0.0, 0.0,
        width(), 0.0,
        0.0, height(),
        width(), height(),
        
        /* rotated fg */
        p[0].x + c.x, p[0].y + c.y,
        p[1].x + c.x, p[1].y + c.y,
        p[2].x + c.x, p[2].y + c.y,
        p[3].x + c.x, p[3].y + c.y,
        
        /* shiny fg */
        -r + c.x, -r + c.y,
         r + c.x, -r + c.y,
        -r + c.x,  r + c.y,
         r + c.x,  r + c.y
    };
    
    pv.bindBuffer();
    pv.setPositions(data, 24);
    pv.unbindBuffer();
}


void ShinyKnob::render()
{
    glEnable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_FOR_GL_ERRORS;
    
    Painter::useCurrent2dProjection();
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    
    pv.bindArray();
    
    Painter::setTexture(bg.id());
    pv.render(GL_TRIANGLE_STRIP, 4, 0);
    
    Painter::setTexture(fg.id());
    pv.render(GL_TRIANGLE_STRIP, 4, 4);
    
    Painter::setTexture(shiny.id());
    pv.render(GL_TRIANGLE_STRIP, 4, 8);
    
    pv.unbindArray();
}


}//namespace r64fx