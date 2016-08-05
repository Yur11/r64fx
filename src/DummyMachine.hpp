#ifndef R64FX_DUMMY_MACHINE_HPP
#define R64FX_DUMMY_MACHINE_HPP

#include "Machine.hpp"

namespace r64fx{
    
class DummyMachine : public Machine{
public:
    DummyMachine(MachinePool* manager);
    
    ~DummyMachine();
    
    void sayHello();
    
protected:
    virtual void dispatchMessage(const MachineMessage &msg);
};
    
}//namespace r64fx

#endif//R64FX_DUMMY_MACHINE_HPP