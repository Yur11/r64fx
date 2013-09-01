#ifndef R64FX_MACHINE_WIDGETS_H
#define R64FX_MACHINE_WIDGETS_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
    
/** @brief A machine shown in the MachineScene. */
class MachineWidget : public Widget{
    Texture _surface_tex;
    
public:
    MachineWidget(Widget* parent = nullptr);
    
    virtual ~MachineWidget();
    
    virtual void render();
};

    
}//namespace r64fx

#endif//R64FX_MACHINE_WIDGETS_H