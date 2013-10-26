#include "machine_widgets.h"
#include "machine_scenes.h"
#include "MouseEvent.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG


using namespace std;


namespace r64fx{

    
MachineWidget::MachineWidget(Widget* parent) : Widget(parent)
{
    resize(19 * 100, 1.75 * 100);
}


MachineWidget::~MachineWidget()
{
}


void MachineWidget::safelySetSurfaceTexture(Texture texture)
{
    surface_texture = texture;
    if(!surface_texture.isGood())
        surface_texture = Texture::defaultTexture();
}


void MachineWidget::render()
{
    glEnable(GL_TEXTURE_2D);
    surface_texture.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(0.0, height() / surface_texture.height());
        glVertex2f(0.0, height());
        
        glTexCoord2f(width() / surface_texture.width(), height() / surface_texture.height());
        glVertex2f(width(), height());
        
        glTexCoord2f(width() / surface_texture.width(), 0.0);
        glVertex2f(width(), 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    Widget::render();
}


void MachineWidget::mousePressEvent(MouseEvent* event)
{
    Widget::mousePressEvent(event);
    if(!event->has_been_handled && event->buttons() & Mouse::Button::Left)
    {
        about_to_be_dragged = true;
        clicked.send(this);
        event->has_been_handled = true;
    }
}

    
void MachineWidget::mouseReleaseEvent(MouseEvent* event)
{
    about_to_be_dragged = false;
    event->has_been_handled = true;
}
    
    
void MachineWidget::mouseMoveEvent(MouseEvent* event)
{
    Widget::mouseMoveEvent(event);
    if(!event->has_been_handled && about_to_be_dragged)
    {
        about_to_be_dragged = false;
        drag_start_mouse_position = event->position();
        dragged.send(this);
        event->has_been_handled = true;
    }
}

}//namespace r64fx 
