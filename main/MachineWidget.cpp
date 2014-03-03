#include "MachineWidget.h"
#include "MachineScene.h"
#include "gui/Error.h"
#include "gui/MouseEvent.h"
#include "gui/Painter.h"
#include "gui/RectVertices.h"
#include "gui/RectPainter.h"

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
    RectPainter::prepare();
    RectPainter::setTexCoords(0.0, 0.0, width() / surface_texture.width(), height() / surface_texture.height());
    RectPainter::setTexture(surface_texture.id());
    RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
    RectPainter::setCoords(0.0, 0.0, width(), height());
    RectPainter::render();
    
    glUseProgram(0);
    CHECK_FOR_GL_ERRORS;
    
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
    Widget::mouseReleaseEvent(event);
    
    if(!event->has_been_handled)
    {
        about_to_be_dragged = false;
        event->has_been_handled = true;
    }
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
