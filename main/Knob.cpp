#include "Knob.h"
#include "gui/Error.h"
#include "gui/MouseEvent.h"
#include "gui/bezier.h"
#include "gui/Window.h"
#include "gui/RectPainter.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
extern string data_prefix;


BasicKnob::BasicKnob(Widget* parent) : Widget(parent)
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


void KnobHandleTypeA::render( Rect<float> rect, float angle, float radius)
{
    glEnable(GL_BLEND);                                 CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  CHECK_FOR_GL_ERRORS;
    
//     glPushMatrix();
    
//     glTranslatef(rect.width() * 0.5, rect.height() * 0.5, 0.0);
//     glScalef(-1.0, 1.0, 1.0);
//     glRotatef(angle, 0.0, 0.0, 1.0);

    rect = rect - radius * 0.3777;
    
    RectPainter::prepare();
    RectPainter::setTexCoords(0.0, 0.0, 1.0, 1.0);
    RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
    
    RectPainter::setTexture(knob_a_base_tex.id());
    RectPainter::setCoords(-rect.width() * 0.5, -rect.height() * 0.5, rect.width(), rect.height());
    RectPainter::render();

    rect = rect - radius * 0.2;
    
//     glRotatef(-angle, 0.0, 0.0, 1.0);
    
    RectPainter::setTexture(knob_a_shiny_tex.id());
    RectPainter::setCoords(-rect.width() * 0.5, -rect.height() * 0.5, rect.width(), rect.height());
    RectPainter::render();

//     glPopMatrix();
    
//     glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);     CHECK_FOR_GL_ERRORS;
}


}//namespace r64fx