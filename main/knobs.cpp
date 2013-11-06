#include "knobs.h"
#include "MouseEvent.h"
#include "bezier.h"
#include "Window.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
extern float light_angle;

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_TEXTURE_2D);
    _tex.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f(rect.width(), 0.0);
        
        glTexCoord2f(1.0, 0.0);
        glVertex2f(rect.width(), rect.height());
        
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, rect.height());
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
}


Texture knob_a_base_tex;
Texture knob_a_shiny_tex;


void KnobHandleTypeA::init()
{
    knob_a_base_tex = Texture("textures/knob_a.png");
    knob_a_shiny_tex = Texture("textures/brushed_metal_knob_top.png");
    knob_a_base_tex.clamp();
    
#ifdef DEBUG
    assert(knob_a_base_tex.isGood());
    assert(knob_a_shiny_tex.isGood());
#endif//DEBUG
}


void KnobHandleTypeA::render(Rect<float> rect, float angle, float radius)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_TEXTURE_2D);
    
    glPushMatrix();
    glTranslatef(rect.width() * 0.5, rect.height() * 0.5, 0.0);
    glScalef(-1.0, 1.0, 1.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    rect = rect - radius * 0.3777;

    knob_a_base_tex.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(-rect.width() * 0.5, -rect.height() * 0.5);
        
        glTexCoord2f(1.0, 0.0);
        glVertex2f( rect.width() * 0.5, -rect.height() * 0.5);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f( rect.width() * 0.5,  rect.height() * 0.5);
        
        glTexCoord2f(0.0, 1.0);
        glVertex2f(-rect.width() * 0.5,  rect.height() * 0.5);
    glEnd();
    
    rect = rect - radius * 0.2;
    
    knob_a_shiny_tex.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    
    glRotatef(-angle, 0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(-rect.width() * 0.5, -rect.height() * 0.5);
        
        glTexCoord2f(1.0, 0.0);
        glVertex2f( rect.width() * 0.5, -rect.height() * 0.5);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f( rect.width() * 0.5,  rect.height() * 0.5);
        
        glTexCoord2f(0.0, 1.0);
        glVertex2f(-rect.width() * 0.5,  rect.height() * 0.5);
    glEnd();
    
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
}


}//namespace r64fx