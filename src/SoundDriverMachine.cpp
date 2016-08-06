#include "SoundDriverMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineGlobalContext.hpp"
#include "SoundDriver.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    constexpr unsigned long Enable             = 1;
    constexpr unsigned long Disable            = 2;
    constexpr unsigned long SetSampleRate      = 3;
    constexpr unsigned long SetBufferSize      = 4;
    constexpr unsigned long CreateAudioInput   = 5;
    constexpr unsigned long CreateAudioOutput  = 6;
    constexpr unsigned long CreateMidiInput    = 7;
    constexpr unsigned long CreateMidiOutput   = 8;
    constexpr unsigned long FreeString         = 9;
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
        if(!ctx()->sound_driver)
        {
            SoundDriver* sd = SoundDriver::newInstance();
            ctx()->sound_driver = sd;
        }
        g_SoundDriverMachineImpl_count++;
    }
    
    virtual void withdraw()
    {
        g_SoundDriverMachineImpl_count--;
        if(g_SoundDriverMachineImpl_count == 0 && ctx()->sound_driver)
        {
            SoundDriver::deleteInstance(ctx()->sound_driver);
            ctx()->sound_driver = nullptr;
        }
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        SoundDriver* sd = ctx()->sound_driver;
        
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
        else if(msg.opcode == CreateAudioInput)
        {
            auto name = (std::string*) msg.value;
            sd->newAudioInput(*name);
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateAudioOutput)
        {
            auto name = (std::string*) msg.value;
            sd->newAudioOutput(*name);
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateMidiInput)
        {
            auto name = (std::string*) msg.value;
            sd->newMidiInput(*name);
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateMidiOutput)
        {
//             auto name = (std::string*) msg.value;
//             sd->newMidiOutput(*name);
            sendMessage(FreeString, msg.value);
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


void SoundDriverMachine::createAudioInput(const std::string &name)
{
    sendMessage(CreateAudioInput, (unsigned long) new std::string(name));
}
    
    
void SoundDriverMachine::createAudioOutput(const std::string &name)
{
    sendMessage(CreateAudioOutput, (unsigned long) new std::string(name));
}


void SoundDriverMachine::createMidiInput(const std::string &name)
{
    sendMessage(CreateMidiInput, (unsigned long) new std::string(name));
}


void SoundDriverMachine::createMidiOutput(const std::string &name)
{
    sendMessage(CreateMidiOutput, (unsigned long) new std::string(name));
}
    
    
void SoundDriverMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == FreeString)
    {
        auto str = (std::string*) msg.value;
        delete str;
    }
}

}//namespace r64fx