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
    constexpr unsigned long SendMidiMessage    = 9;
    constexpr unsigned long RouteThrough       = 10;
    constexpr unsigned long FreeString         = 11;
    constexpr unsigned long FreeStringPair     = 12;
    
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
            auto name = (std::string*) msg.value;
            sd->newMidiOutput(*name);
            sendMessage(FreeString, msg.value);
        }
        else if(msg.opcode == SendMidiMessage)
        {
            SoundDriverIOPort* port = sd->findPort("midi_out");
            if(port)
            {
                auto midi_port = dynamic_cast<SoundDriverIOPort_MidiOutput*>(port);
                if(midi_port)
                {
                    MidiMessage midi_msg(msg.value);
                    MidiEvent event(midi_msg, 0);
                    midi_port->writeEvents(&event, 1);
                }
            }
        }
        else if(msg.opcode == RouteThrough)
        {
            if(!sg)
            {
                sg = new SignalGraph;
            }
            
            auto p = (pair<std::string, std::string>*) msg.value;
            auto &output = p->first;
            auto &input = p->second;
            
            SoundDriverIOPort* output_port = sd->findPort(output);
            SoundDriverIOPort* input_port = sd->findPort(input);
            if(output_port && input_port)
            {
                auto audio_output = dynamic_cast<SoundDriverIOPort_AudioOutput*>(output_port);
                auto audio_input = dynamic_cast<SoundDriverIOPort_AudioInput*>(input_port);
                if(audio_output && audio_input)
                {
                    cout << sd->bufferSize() << "\n";
                    auto reader = new SignalNode_BufferReader(audio_input, sd->bufferSize());
                    auto writer = new SignalNode_BufferWriter(audio_output, sd->bufferSize());
                    cout << reader->source().addr() << ", " << writer->sink().addr() << "\n";
                    auto connection = new SignalConnection(reader->source(), writer->sink());

                    sg->addNode(reader);
                    sg->addNode(writer);
                    sg->addConnection(connection);
                }
            }
            
            sendMessage(FreeStringPair, (unsigned long)p);
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
    
    
void SoundDriverMachine::sendMidiMessage(MidiMessage msg)
{
    sendMessage(SendMidiMessage, (unsigned long)msg);
}


void SoundDriverMachine::routeThrough(const std::string &output, const std::string &input)
{
    auto p = new pair<std::string, std::string>(output, input);
    sendMessage(RouteThrough, (unsigned long)p);
}
    
    
void SoundDriverMachine::dispatchMessage(const MachineMessage &msg)
{
    if(msg.opcode == FreeString)
    {
        auto str = (std::string*) msg.value;
        delete str;
    }
    else if(msg.opcode == FreeStringPair)
    {
        auto p = (pair<std::string, std::string>*) msg.value;
        delete p;
    }
}

}//namespace r64fx