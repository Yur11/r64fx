#include "Machine.h"
#include "gui/Texture.h"
#include "gui/Dummy.h"


namespace r64fx{
    
Machine::Machine(/*Graph* graph, */FrontMachineScene* fms, BackMachineScene* bms)
: _fms(fms)
, _bms(bms)
{
    _front = new MachineWidget;
    _back = new MachineWidget;
    
    _front->safelySetSurfaceTexture(Texture("textures/grainy_greenish.png"));
    _back->safelySetSurfaceTexture(Texture("textures/grainy_dark.png"));
    
    _front->clicked = Message([](void* source, void* data)->void*{
        auto machine = (Machine*) data;
        machine->frontScene()->selectWidget(machine->front());
        machine->backScene()->selectWidget(machine->back());
        return nullptr;
    }, this);
    
    _back->clicked = Message([](void* source, void* data)->void*{
        auto machine = (Machine*) data;
        machine->frontScene()->selectWidget(machine->front());
        machine->backScene()->selectWidget(machine->back());
        return nullptr;
    }, this);
    
    _front->dragged = Message([](void*, void* data)->void*{
        auto machine = (Machine*) data;
        machine->frontScene()->startDrag();
        machine->backScene()->startDrag();
        return nullptr;
    }, this);
    
    _back->dragged = Message([](void*, void* data)->void*{
        auto machine = (Machine*) data;
        machine->frontScene()->startDrag();
        machine->backScene()->startDrag();
        return nullptr;
    }, this);
    
    _fms->appendWidget(_front);
    _bms->appendWidget(_back);
}


Machine::~Machine()
{
    delete _front;
    delete _back;
}


void Machine::setPosition(Point<float> p)
{
    _front->setPosition(p);
    _back->setPosition(p);
}
    
}//namespace r64fx