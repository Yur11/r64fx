#include "Knob.h"
#include "gui/Error.h"
#include "gui/MouseEvent.h"
#include "gui/bezier.h"
#include "gui/Window.h"
#include "gui/Painter.h"
#include "gui/RectPainter.h"
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

        this->rotated();
        value_changed.send(this);
        
        event->has_been_handled = true;
    }
}


TexturedKnobBackground::TexturedKnobBackground(Texture tex)
: _tex(tex)
{
#ifdef DEBUG
    assert(_tex.isGood());
#endif//DEBUG
}


void TexturedKnobBackground::render(Rect<float> rect)
{
    glEnable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_FOR_GL_ERRORS;
    
    RectPainter::prepare();
    RectPainter::setTexCoords(0.0, 0.0, 1.0, -1.0);
    RectPainter::setTexture(_tex.id());
    RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
    RectPainter::setCoords(0.0, 0.0, rect.width(), rect.height());
    RectPainter::render();
        
    glDisable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
}


Texture knob_a_base_tex;
Texture knob_a_shiny_tex;


void KnobHandleTypeA::init()
{
    knob_a_base_tex = Texture(data_prefix + "textures/knob_a.png");
    knob_a_shiny_tex = Texture(data_prefix + "textures/brushed_metal_knob_top.png");
    glActiveTexture(GL_TEXTURE0 + 0);
    
#ifdef DEBUG
    assert(knob_a_base_tex.isGood());
    assert(knob_a_shiny_tex.isGood());
#endif//DEBUG
}


KnobHandleTypeA::KnobHandleTypeA() : pv(8)
{
    static float data[16] = {
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
    pv.setTexCoords(data, 16);
    pv.unbindBuffer();
}


void KnobHandleTypeA::update(Point<float> center, float angle, float radius)
{
    radius *= 0.6;
    
    Point<float> p[4] = {
        { -radius, -radius },
        {  radius, -radius },
        { -radius,  radius },
        {  radius,  radius }
    };
    
    for(int i=0; i<4; i++)
    {
        p[i].rotate( - angle * M_PI / 180);
    }

    radius *= 0.7;

    float data[16] = {
        p[0].x + center.x, p[0].y + center.y,
        p[1].x + center.x, p[1].y + center.y,
        p[2].x + center.x, p[2].y + center.y,
        p[3].x + center.x, p[3].y + center.y,
        
        -radius + center.x, -radius + center.y,
         radius + center.x, -radius + center.y,
        -radius + center.x,  radius + center.y,
         radius + center.x,  radius + center.y
    };
    
    pv.bindBuffer();
    pv.setPositions(data, 16);
    pv.unbindBuffer();
}


void KnobHandleTypeA::render()
{
    glEnable(GL_BLEND);                                 CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  CHECK_FOR_GL_ERRORS;
    
    Painter::enable();
    Painter::useCurrent2dProjection();
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    Painter::setTexturingMode(Painter::RGBA);
    
    pv.bindArray();
    
    Painter::setTexture(knob_a_base_tex.id());
    pv.render(GL_TRIANGLE_STRIP, 4);
    
    Painter::setTexture(knob_a_shiny_tex.id());
    pv.render(GL_TRIANGLE_STRIP, 4, 4);
    
    pv.unbindArray();
    
    Painter::disable();
    
    glDisable(GL_BLEND);     CHECK_FOR_GL_ERRORS;
}


}//namespace r64fx