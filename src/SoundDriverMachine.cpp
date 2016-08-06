#include "SoundDriverMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineGlobalContext.hpp"
#include "SoundDriver.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    constexpr unsigned long Enable         = 1;
    constexpr unsigned long Disable        = 2;
    constexpr unsigned long SetSampleRate  = 3;
    constexpr unsigned long SetBufferSize  = 4;
}//namespace
    
int g_SoundDriverMachineImpl_count = 0;
    
class SoundDriverMachineImpl : public MachineImpl{
public:
    SoundDriverMachineImpl(Machine* iface)
    : MachineImpl(iface)
    {
        
    }
    
    virtual ~SoundDriverMachineImpl()
    {
        
    }
    
protected:
    virtual void deploy()
    {
        cout << "sd deploy\n";
        if(!ctx()->soundDriver())
        {
            SoundDriver* sd = SoundDriver::newInstance();
            ctx()->setSoundDriver(sd);
        }
        g_SoundDriverMachineImpl_count++;
    }
    
    virtual void withdraw()
    {
        cout << "sd withdraw\n";
        g_SoundDriverMachineImpl_count--;
        if(g_SoundDriverMachineImpl_count == 0 && ctx()->soundDriver())
        {
            SoundDriver::deleteInstance(ctx()->soundDriver());
            ctx()->setSoundDriver(nullptr);
        }
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        SoundDriver* sd = ctx()->soundDriver();
        
        if(msg.opcode == Enable)
        {
            sd->enable();
        }
        else if(msg.opcode == Disable)
        {
            sd->disable();
        }
        else if(msg.opcode == SetSampleRate)
        {
//             sd->setSampleRate(msg.value);
        }
        else if(msg.opcode == SetBufferSize)
        {
//             sd->setBufferSize(msg.value);
        }
    }
};
    

SoundDriverMachine::SoundDriverMachine(MachinePool* pool)
: Machine(pool)
{
    auto impl = new SoundDriverMachineImpl(this);
    setImpl(impl);
}
    
    
SoundDriverMachine::~SoundDriverMachine()
{
    
}
    
    
void SoundDriverMachine::enable()
{
    sendMessage(Enable, 0);
}
    
    
void SoundDriverMachine::disable()
{
    sendMessage(Disable, 0);
}


void SoundDriverMachine::setSampleRate(int sample_rate)
{
    sendMessage(SetSampleRate, sample_rate);
}


void SoundDriverMachine::setBufferSize(int buffer_size)
{
    sendMessage(SetBufferSize, buffer_size);
}
    
    
void SoundDriverMachine::dispatchMessage(const MachineMessage &msg)
{
    
}

}//namespace r64fx