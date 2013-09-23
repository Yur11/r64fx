#include "Machine.h"

#include "Dummy.h"

namespace r64fx{
    
    
Machine::Machine(/*Graph* graph, */FrontMachineScene* fms, BackMachineScene* bms)
: _fms(fms)
, _bms(bms)
{
    _front = new MachineWidget;
    _back = new MachineWidget;
    
    _front->safelySetSurfaceTexture(Texture("textures/grainy_greenish.png"));
    _back->safelySetSurfaceTexture(Texture("textures/grainy_dark.png"));
    
    _front->dragged= Message([](void*, void* data)->void*{
        auto machine = (Machine*) data;
        machine->_fms->startDrag(machine->_front, machine->_back);
        return nullptr;
    }, this);
    
    _back->dragged = Message([](void*, void* data)->void*{
        auto machine = (Machine*) data;
        machine->_bms->startDrag(machine->_back, machine->_front);
        return nullptr;
    }, this);
    
    _front->setPosition(100.0, 200.0);
    _back->setPosition(100.0, 200.0);
    
    _fms->appendWidget(_front);
    _bms->appendWidget(_back);
}


Machine::~Machine()
{
    delete _front;
    delete _back;
}
    
}//namespace r64fx