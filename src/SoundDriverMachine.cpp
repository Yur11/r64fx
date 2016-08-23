#include "SoundDriverMachine.hpp"
#include "MachineImpl.hpp"
#include "MachinePortImpl.hpp"
#include "MachinePoolContext.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"
#include "SignalNode_BufferIO.hpp"
#include "StringUtils.hpp"

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
    constexpr unsigned long DestroyAudioInput  = 9;
    constexpr unsigned long DestroyAudioOutput = 10;
    constexpr unsigned long DestroyMidiInput   = 11;
    constexpr unsigned long DestroyMidiOutput  = 12;
    constexpr unsigned long Connect    = 13;
    constexpr unsigned long Disconnect = 14;
    
    constexpr unsigned long SignalSourceCreated   = 15;
    constexpr unsigned long SignalSinkCreated     = 16;
    
    struct CreatePortSpec{
        std::string port_name  = "";
        unsigned long component_count = 0;
        
        void* impl = nullptr;
        void* port = nullptr;
    };
    
    struct PortConnectionSpec{
        std::string src = "";
        std::string dst = "";
    };
}//namespace
    
    
class SoundDriverSignalSource : public SignalSource{
    
};


class SoundDriverSignalSink : public SignalSink{
    
};
    
    
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
        auto sound_driver  = ctx()->sound_driver;
        
        if(msg.opcode == Enable)
        {
            sound_driver->enable();
        }
        else if(msg.opcode == Disable)
        {
            sound_driver->disable();
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
            auto spec = (CreatePortSpec*) msg.value;
            
            auto source_impl = new MachineSourceImpl;
            source_impl->sources.resize(spec->component_count);
            for(unsigned long i=0; i<spec->component_count; i++)
            {
                string name = spec->port_name;
                if(spec->component_count > 1)
                {
                    name += "_" + num2str(i + 1);
                }

                auto input = sound_driver->newAudioInput(name);
                auto node =  new SignalNode_BufferReader(input, sound_driver->bufferSize());
                ctx()->input_subgraph->addItem(node);
                source_impl->sources.at(i) = node->source();
            }
            
            spec->impl = source_impl;
            sendMessage(SignalSourceCreated, (unsigned long)spec);
        }
        else if(msg.opcode == CreateAudioOutput)
        {
            auto spec = (CreatePortSpec*) msg.value;
            
            auto sink_impl = new MachineSinkImpl;
            sink_impl->sinks.resize(spec->component_count);
            for(unsigned long i=0; i<spec->component_count; i++)
            {
                string name = spec->port_name;
                if(spec->component_count > 1)
                {
                    name += "_" + num2str(i + 1);
                }

                auto output = sound_driver->newAudioOutput(name);
                auto node =  new SignalNode_BufferWriter(output, sound_driver->bufferSize());
                ctx()->output_subgraph->addItem(node);
                sink_impl->sinks.at(i) = node->sink();
            }
            
            spec->impl = sink_impl;
            sendMessage(SignalSourceCreated, (unsigned long)spec);
        }
        else if(msg.opcode == CreateMidiInput)
        {
        }
        else if(msg.opcode == CreateMidiOutput)
        {
        }
        else if(msg.opcode == DestroyAudioInput)
        {
            auto source_impl = (MachineSourceImpl*) msg.value;
            for(unsigned long i=0; source_impl->sources.size(); i++)
            {
                auto source = (BufferReaderSignalSource*) source_impl->sources.at(i);
                auto node = source->parentReader();
                ctx()->input_subgraph->removeItem(node);
                sound_driver->deletePort(node->input());
                delete node;
            }
            delete source_impl;
        }
        else if(msg.opcode == DestroyAudioOutput)
        {
            auto sink_impl = (MachineSinkImpl*) msg.value;
            for(unsigned long i=0; i<sink_impl->sinks.size(); i++)
            {
                auto sink = (BufferWriterSignalSink*) sink_impl->sinks.at(i);
                auto node = sink->parentWriter();
                ctx()->output_subgraph->removeItem(node);
                sound_driver->deletePort(node->output());
                delete node;
            }
            delete sink_impl;
        }
        else if(msg.opcode == DestroyMidiInput)
        {
        }
        else if(msg.opcode == DestroyMidiOutput)
        {
        }
        else if(msg.opcode == Connect)
        {
            auto spec = (PortConnectionSpec*)msg.value;
            sound_driver->connect(spec->src, spec->dst);
        }
        else if(msg.opcode == Disconnect)
        {
            auto spec = (PortConnectionSpec*)msg.value;
            sound_driver->disconnect(spec->src, spec->dst);
        }
    }
    
    virtual void cycleStarted()
    {
        
    }
    
    virtual void cycleEnded()
    {
        
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


MachineSignalSource* SoundDriverMachine::createAudioInput(const std::string &name, int component_count)
{
    auto source = new MachineSignalSource(this, name, component_count);
    
    auto spec = new CreatePortSpec;
    spec->port = source;
    spec->port_name = name;
    spec->component_count = component_count;
    sendMessage(CreateAudioInput, (unsigned long)spec);
    
    m_ports.append(source);
    return source;
}
    
    
MachineSignalSink* SoundDriverMachine::createAudioOutput(const std::string &name, int component_count)
{
    auto sink = new MachineSignalSink(this, name, component_count);
    
    auto spec = new CreatePortSpec;
    spec->port = sink;
    spec->port_name = name;
    spec->component_count = component_count;
    sendMessage(CreateAudioOutput, (unsigned long)spec);
    
    m_ports.append(sink);
    return sink;
}


void SoundDriverMachine::createMidiInput(const std::string &name)
{
//     sendMessage(CreateMidiInput, (unsigned long) new std::string(name));
//     m_ports.append(new MachinePort(this, name, true, false));
}


void SoundDriverMachine::createMidiOutput(const std::string &name)
{
//     sendMessage(CreateMidiOutput, (unsigned long) new std::string(name));
//     m_ports.append(new MachinePort(this, name, false, false));
}


void SoundDriverMachine::destroyPort(MachinePort* port)
{
    if(port->isSignalPort())
    {
        if(port->isSource())
        {
            auto source = (MachineSignalSource*)port;
            if(source->impl())
            {
                sendMessage(DestroyAudioInput, (unsigned long)source->impl());
            }
        }
        else if(port->isSink())
        {
            auto sink = (MachineSignalSink*)port;
            if(sink->impl())
            {
                sendMessage(DestroyAudioOutput, (unsigned long)sink->impl());
            }
        }
    }
    else if(port->isSequencerPort())
    {
        
    }
}


void SoundDriverMachine::clear()
{
    for(auto port : m_ports)
    {
        destroyPort(port);
    }
}


void SoundDriverMachine::connect(const std::string &src, const std::string &dst)
{
    auto spec = new PortConnectionSpec;
    spec->src = src;
    spec->dst = dst;
    sendMessage(Connect, (unsigned long)spec);
}


void SoundDriverMachine::disconnect(const std::string &src, const std::string &dst)
{
    auto spec = new PortConnectionSpec;
    spec->src = src;
    spec->dst = dst;
    sendMessage(Disconnect, (unsigned long)spec);
}
      
    
void SoundDriverMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == SignalSourceCreated)
    {
        auto spec = (CreatePortSpec*) msg.value;
        auto source = (MachineSignalSource*) spec->port;
        auto impl = (MachineSourceImpl*) spec->impl;
        source->setImpl(impl);
        delete spec;
    }
    else if(msg.opcode == SignalSinkCreated)
    {
        auto spec = (CreatePortSpec*) msg.value;
        auto sink = (MachineSignalSink*) spec->port;
        auto impl = (MachineSinkImpl*) spec->impl;
        sink->setImpl(impl);
        delete spec;
    }
}

}//namespace r64fx