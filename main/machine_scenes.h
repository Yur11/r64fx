#ifndef R64FX_MACHINE_SCENES_H
#define R64FX_MACHINE_SCENES_H

#include "Scene.h"
#include "machine_widgets.h"

namespace r64fx{
    

class MachineScene : public Scene{
    MachineWidget* dragged_widget = nullptr;
    Point<float> mouse_position = {0.0, 0.0};
    
    /** @brief Can we drop the dragged widget at the current mouse position. */
    bool canDropWidget();
    
public:
    MachineScene* counterpart_scene = nullptr;
    
    virtual void render();

    virtual void mouseReleaseEvent(MouseEvent*);
    
    virtual void mouseMoveEvent(MouseEvent*);
    
    virtual void keyPressEvent(KeyEvent*);
    
    virtual void keyReleaseEvent(KeyEvent*);
    
    void startDrag(MachineWidget* own_dragged_widget, MachineWidget* counterpart_scene_widget);
};
    
    
/** @brief Scene with machines and wires. Front. */
class FrontMachineScene : public MachineScene{    
};


class BackMachineScene : public MachineScene{
};


    
}//namespace r64fx

#endif//R64FX_MACHINE_SCENES_H