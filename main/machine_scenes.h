#ifndef R64FX_MACHINE_SCENES_H
#define R64FX_MACHINE_SCENES_H

#include "Scene.h"
#include "machine_widgets.h"

namespace r64fx{
    

class MachineScene : public Scene{
    
public:
    MachineScene* counterpart_scene = nullptr;
    
    virtual void keyPressEvent(KeyEvent*);
    
    virtual void keyReleaseEvent(KeyEvent*);
};
    
    
/** @brief Scene with machines and wires. Front. */
class FrontMachineScene : public MachineScene{    
};


class BackMachineScene : public MachineScene{
};


    
}//namespace r64fx

#endif//R64FX_MACHINE_SCENES_H