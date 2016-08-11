#include "SoundDriverMachine.hpp"
#include "MachineImpl.hpp"
#include "MachinePoolContext.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"
#include "SignalNode_BufferIO.hpp"

#include <utility>

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
    constexpr unsigned long PortCreated        = 10;
    constexpr unsigned long RemovePort         = 11;
}//namespace
    
    
class SoundDriverMachineImpl : public MachineImpl{
public:    
    virtual ~SoundDriverMachineImpl()
    {
        
    }
    
protected:
    virtual void deploy()
    {
    }
    
    virtual void withdraw()
    {
    }
    
    virtual void dispatchMessage(const MachineMessage &msg)
    {
        SoundDriver* &sd = ctx()->sound_driver;
        SignalGraph* &sg = ctx()->signal_graph;
        
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
            auto port = sd->newAudioInput(*name);
            if(port)
            {
                auto reader = new SignalNode_BufferReader(port, sd->bufferSize(), sg);
                sendMessage(PortCreated, (unsigned long)reader->source());
            }
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateAudioOutput)
        {
            auto name = (std::string*) msg.value;
            auto port = sd->newAudioOutput(*name);
            if(port)
            {
                auto writer = new SignalNode_BufferWriter(port, sd->bufferSize(), sg);
                sendMessage(PortCreated, (unsigned long)writer->sink());
            }
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateMidiInput)
        {
//             auto name = (std::string*) msg.value;
//             auto port = sd->newMidiInput(*name);
//             sendMessage(PortCreated, (unsigned long)port);
//             sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == CreateMidiOutput)
        {
//             auto name = (std::string*) msg.value;
//             auto port = sd->newMidiOutput(*name);
//             sendMessage(PortCreated, (unsigned long)port);
//             sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == RemovePort)
        {
//             auto port = (SoundDriverIOPort*) msg.value;
//             delete port;
        }
    }
};
    

SoundDriverMachine::SoundDriverMachine(MachinePool* pool)
: Machine(pool)
{
    setImpl(new SoundDriverMachineImpl);
}
    
    
SoundDriverMachine::~SoundDriverMachine()
{
    clear();
}


void SoundDriverMachine::forEachPort(void (*fun)(MachinePort* port, Machine* machine, void* arg), void* arg)
{
    for(auto port : m_ports)
    {
        fun(port, this, arg);
    }
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


MachineSignalSource* SoundDriverMachine::createAudioInput(const std::string &name)
{
    sendMessage(CreateAudioInput, (unsigned long) new std::string(name));
    auto input = new MachineSignalSource(this, name);
    m_ports.append(input);
    return input;
}
    
    
MachineSignalSink* SoundDriverMachine::createAudioOutput(const std::string &name)
{
    sendMessage(CreateAudioOutput, (unsigned long) new std::string(name));
    auto output = new MachineSignalSink(this, name);
    m_ports.append(output);
    return output;
}


void SoundDriverMachine::createMidiInput(const std::string &name)
{
    sendMessage(CreateMidiInput, (unsigned long) new std::string(name));
//     m_ports.append(new MachinePort(this, name, true, false));
}


void SoundDriverMachine::createMidiOutput(const std::string &name)
{
    sendMessage(CreateMidiOutput, (unsigned long) new std::string(name));
//     m_ports.append(new MachinePort(this, name, false, false));
}


void SoundDriverMachine::clear()
{
    for(auto port : m_ports)
    {
        sendMessage(RemovePort, (unsigned long) port->handle());
    }
}
      
    
void SoundDriverMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == FreeString)
    {
        auto str = (std::string*) msg.value;
        delete str;
    }
    else if(msg.opcode == PortCreated)
    {
        auto handle = (void*)msg.value;
        for(auto port : m_ports)
        {
            if(!port->handle())
            {
                port->setHandle(handle);
                break;
            }
        }
    }
}

}//namespace r64fx