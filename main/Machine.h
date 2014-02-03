#ifndef R64FX_MACHINE_H
#define R64FX_MACHINE_H

#include "MachineScene.h"
#include "dsp/Graph.h"

namespace r64fx{
    
class Machine{
    FrontMachineScene* _fms;
    BackMachineScene* _bms;
    
    MachineWidget* _front;
    MachineWidget* _back;
    
public:
    Machine(FrontMachineScene* fms, BackMachineScene* bms);

    virtual ~Machine();
    
    inline FrontMachineScene* frontScene() const { return _fms; }
    
    inline BackMachineScene* backScene() const { return _bms; }
    
    inline MachineWidget* front() const { return _front; }
    
    inline MachineWidget* back() const { return _back; }
    
    void setPosition(Point<float> p);
    
    inline void setPosition(float x, float y) { setPosition(Point<float>(x, y)); }
    
    inline Point<float> position() const { return _front->position(); }
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_H