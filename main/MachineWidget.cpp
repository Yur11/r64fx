#include "MachineWidget.h"
#include "MachineScene.h"
#include "gui/MouseEvent.h"
#include "gui/Painter.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG


using namespace std;


namespace r64fx{

MachineWidget::MachineWidget(Widget* parent) : Widget(parent), p(4)
{
    resize(19 * 100, 1.75 * 100);
    update();
}


MachineWidget::~MachineWidget()
{
}

void MachineWidget::update()
{
    float texw = float(width()) / 64.0;
    float texh = float(height()) / 64.0;
    
    static float data[16] = {
        0.0, 0.0,
        width(), 0.0,
        0.0, height(),
        width(), height(),
        
        0.0, 0.0,
        texw, 0.0,
        0.0, texh,
        texw, texh
    };
    
    p.bindBuffer();
    p.setData(data);
    p.unbindBuffer();
}


void MachineWidget::render()
{
//     RectPainter::prepare();
//     RectPainter::setTexCoords(0.0, 0.0, width() / surface_texture.width(), height() / surface_texture.height());
//     RectPainter::setTexture(surface_texture.id());
//     RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
//     RectPainter::setCoords(0.0, 0.0, width(), height());
//     RectPainter::render();
//     
//     glUseProgram(0);
//     CHECK_FOR_GL_ERRORS;
    
//     Painter::enable();
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    Painter::setTexture(surface_texture->glName());
//     Painter::useCurrent2dProjection();
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
    
//     Painter::disable();
    
    Widget::render();
}


void MachineWidget::mousePressEvent(MouseEvent* event)
{
    Widget::mousePressEvent(event);
    if(!event->has_been_handled && event->buttons() & Mouse::Button::Left)
    {
        about_to_be_dragged = true;
//         clicked.send(this);
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
//         dragged.send(this);
        event->has_been_handled = true;
    }
}

}//namespace r64fx 
