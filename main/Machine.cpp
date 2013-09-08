#include "Machine.h"

#include "Dummy.h"

namespace r64fx{
    
Machine::Machine(/*Graph* graph, */FrontMachineScene* fms, BackMachineScene* bms)
: _fms(fms)
, _bms(bms)
{
    _front = new MachineWidget;
    _back = new MachineWidget;
    
//     _front->setPosition(100.0, 200.0);
//     _back->setPosition(100.0, 200.0);
    
    auto dummy = new Dummy(100, 100);
    dummy->setPosition(100, 100);
    
    _fms->appendWidget(dummy);
    
    _fms->appendWidget(_front);
    _bms->appendWidget(_back);
}


Machine::~Machine()
{
    delete _front;
    delete _back;
}
    
}//namespace r64fx