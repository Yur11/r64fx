#ifndef R64FX_MACHINE_WIDGETS_H
#define R64FX_MACHINE_WIDGETS_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
    
/** @brief A machine shown in the MachineScene. */
class MachineWidget : public Widget{
    bool about_to_be_dragged = false;
    
public:
    MachineWidget(Widget* parent = nullptr);

    Texture surface_texture;
    
    /** @brief Set a texture to be used as a surface texture for this widget. 
     
        Will set the default texture if the texture given is not good.
     */
    void safelySetSurfaceTexture(Texture texture);
    
    virtual ~MachineWidget();
    
    virtual void render();

    Message dragged;
    
    /** @brief Mouse position where the drag has started, in the coordinate system of this widget. */
    Point<float> drag_start_mouse_position;
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};

    
}//namespace r64fx

#endif//R64FX_MACHINE_WIDGETS_H