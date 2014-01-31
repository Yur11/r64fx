#include "knobs.h"
#include "gui/MouseEvent.h"
#include "gui/bezier.h"
#include "gui/Window.h"
#include "gui/TexturedRect.h"

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


void TexturedKnobBackground::render(RenderingContextId_t context_id, Rect<float> rect)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    TexturedRect::render(context_id, 0.0, 0.0, rect.width(), rect.height(), 0.0, 0.0, 1.0, -1.0, _tex.id());
    glUseProgram(0);
    
    glDisable(GL_BLEND);
}


Texture knob_a_base_tex;
Texture knob_a_shiny_tex;


void KnobHandleTypeA::init()
{
    knob_a_base_tex = Texture(data_prefix + "textures/knob_a.png");
    knob_a_shiny_tex = Texture(data_prefix + "textures/brushed_metal_knob_top.png");
    knob_a_base_tex.clamp();
    
#ifdef DEBUG
    assert(knob_a_base_tex.isGood());
    assert(knob_a_shiny_tex.isGood());
#endif//DEBUG
}


void KnobHandleTypeA::render(RenderingContextId_t context_id, Rect<float> rect, float angle, float radius)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
    glTranslatef(rect.width() * 0.5, rect.height() * 0.5, 0.0);
    glScalef(-1.0, 1.0, 1.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    rect = rect - radius * 0.3777;
    
    TexturedRect::render(
        context_id,
        -rect.width() * 0.5, -rect.height() * 0.5, rect.width(), rect.height(),
        0.0, 0.0, 1.0, 1.0,
        knob_a_base_tex.id()
    );
    
    rect = rect - radius * 0.2;
    
    glRotatef(-angle, 0.0, 0.0, 1.0);
    
    TexturedRect::render(
        context_id,
        -rect.width() * 0.5, -rect.height() * 0.5, rect.width(), rect.height(),
        0.0, 0.0, 1.0, 1.0,
        knob_a_shiny_tex.id()
    );
    glUseProgram(0);
    
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
}


}//namespace r64fx