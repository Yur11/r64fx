#include "Machine.h"

namespace r64fx{
    
Machine::Machine(/*Graph* graph, */FrontMachineScene* fms, BackMachineScene* bms)
: _fms(fms)
, _bms(bms)
{
    _front = new MachineWidget;
    _back = new MachineWidget;
    
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