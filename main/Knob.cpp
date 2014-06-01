#include "Knob.h"
#include "gui/MouseEvent.h"
#include "gui/bezier.h"
#include "gui/Window.h"
#include "gui/Painter.h"
#include <cmath>

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

    
void BasicKnob::mousePressEvent(MouseEvent* event)
{
    if(!Widget::isMouseInputGrabber())
        grabMouseInput();
//     Mouse::setRelativeMode(true);
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
    if(Widget::isMouseInputGrabber())
    {
        angle -= Mouse::relativePosition().y * 2;
        if(angle < min_angle)
            angle = min_angle;
        else if(angle > max_angle)
            angle = max_angle;
        
        appearanceChangeEvent();
        event->has_been_handled = true;
    }
}


Knob::Knob(Texture2D* background, Texture2D* rotatable, Texture2D* foreground)
: p(12)
, background_tex(background)
, rotatable_tex(rotatable)
, foreground_tex(foreground)
{
    static float data[24] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        
        0.0, 1.0,
        1.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
    };
    
    p.bindBuffer();
    p.setTexCoords(data, 24);
    p.unbindBuffer();
}


Knob::Knob(std::string background, std::string rotatable, std::string foreground)
: Knob(
    (Texture2D*) Texture::find(background),
    (Texture2D*) Texture::find(rotatable),
    (Texture2D*) Texture::find(foreground)
){}


void rotate_points(Point<float>* points, int npoints, float angle)
{
    auto rad = (angle * M_PI) / 180.0;
    
    auto cosang = cos(rad);
    auto sinang = sin(rad);
    
    for(auto i=0; i<npoints; i++)
    {
        auto &p = points[i];
        float x = p.x * cosang - p.y * sinang;
        float y = p.x * sinang + p.y * cosang;
        p.x = x;
        p.y = y;
    }
}


void Knob::appearanceChangeEvent()
{
    auto r = projectedRect();
    
    float data[24];
    
    data[0] = data[8] = data[16] = r.left;
    data[1] = data[9] = data[17] = r.top;
    
    data[2] = data[10] = data[18] = r.right;
    data[3] = data[11] = data[19] = r.top;
    
    data[4] = data[12] = data[20] = r.left;
    data[5] = data[13] = data[21] = r.bottom;
    
    data[6] = data[14] = data[22] = r.right;
    data[7] = data[15] = data[23] = r.bottom;
    
    Rect<float> rr = {
        - r.width() * 0.5f,
        - r.height() * 0.5f,
        + r.width() * 0.5f,
        + r.height() * 0.5f,
    };
        
    Point<float> pp[4] = {
        { rr.left, rr.top },
        { rr.right, rr.top },
        { rr.left, rr.bottom },
        { rr.right, rr.bottom }
    };
    
    rotate_points(pp, 4, angle);
    
    for(int i=0; i<4; i++)
    {
        pp[i] += r.center();
        data[i*2 + 8] = pp[i].x;
        data[i*2 + 9] = pp[i].y;
    }
    
    p.bindBuffer();
    p.setPositions(data, 24);
    p.unbindBuffer();
}


void Knob::render()
{    
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    
    p.bindArray();
    
    Painter::setTexture(background_tex->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 0);
    
    Painter::setTexture(rotatable_tex->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 4);
    
    Painter::setTexture(foreground_tex->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 8);
    
    p.unbindArray();    
}


}//namespace r64fx