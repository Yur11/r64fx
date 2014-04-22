#ifndef R64FX_MAIN_MACHINE_WIDGET_H
#define R64FX_MAIN_MACHINE_WIDGET_H

#include "gui/Widget.h"
#include "gui/Texture.h"
#include "gui/Painter.h"

namespace r64fx{
    
    
/** @brief A machine shown in the MachineScene. */
class MachineWidget : public Widget{
    bool about_to_be_dragged = false;
    Painter p;
    
public:
    MachineWidget(Widget* parent = nullptr);
    
    Texture2D* surface_texture = nullptr;
    
    virtual ~MachineWidget();
    
    void update();
    
    virtual void render();
    
//     Message clicked;
    
//     Message dragged;
    
    /** @brief Mouse position where the drag has started, in the coordinate system of this widget. */
    Point<float> drag_start_mouse_position;
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};

    
}//namespace r64fx

#endif//R64FX_MACHINE_WIDGET_H