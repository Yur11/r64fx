#include <string>
#include "Machine.h"
#include "gui/Texture.h"
#include "gui/Dummy.h"

#include <iostream>

using namespace std;

namespace r64fx{
    
extern string data_prefix;
    
Machine::Machine(FrontMachineScene* fms, BackMachineScene* bms)
: _fms(fms)
, _bms(bms)
{
    _front = new MachineWidget;
    _back = new MachineWidget;
    
    _front->safelySetSurfaceTexture(Texture("./data/textures/grainy_greenish.png"));
    _back->safelySetSurfaceTexture(Texture("./data/textures/grainy_dark.png"));
    
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


void Machine::addSocket(Socket* socket)
{
    _back->appendWidget(socket);
    socket->clicked = { [](void* source, void* data) -> void* {
        auto machine = (Machine*) data;
        machine->backScene()->handleSocketClick((Socket*)source);
        return nullptr;
    }, this};
}
    
}//namespace r64fx