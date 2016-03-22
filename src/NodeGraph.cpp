#include "NodeGraph.hpp"
#include "SoundDriver.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "sleep.hpp"
#include <iostream>
#include <cmath>

#define m_impl ((NodeGraphImpl*)m)


using namespace std;

namespace r64fx{


enum class NodeGraphCommand{
    Quit
};


struct NodeGraphImpl{
    SoundDriver* driver = nullptr;
    Thread thread;
    CircularBuffer<NodeGraphCommand> commands;
    bool is_enabled = false;

    NodeGraphImpl()
    : commands(16)
    {

    }

    void enable()
    {
        thread.run([](void* arg) -> void*{
            auto self = (NodeGraphImpl*)arg;
            return self->process();
        }, this);

        is_enabled = true;
    }


    void disable()
    {
        commands.write(NodeGraphCommand::Quit);
        thread.join();
        is_enabled = false;
    }


    void* process();
};


void* NodeGraphImpl::process()
{
    SoundDriverIOStatusPort* status_port = driver->newStatusPort();


    auto audio_out = driver->newAudioOutput("audio_out");
    if(!audio_out)
    {
        cerr << "Failed to create audio output!\n";
    }

    auto midi_in = driver->newMidiInput("midi_in");
    if(!midi_in)
    {
        cerr << "Failed to create midi input!\n";
    }

    float* buffer = new float[driver->bufferSize()];
    for(int i=0; i<driver->bufferSize(); i++)
    {
        buffer[i] = 0.0f;
    }

    float freq1 = 440.0f;
    float freq2 = 440.0f;
    float t1 = 0.0f;
    float t2 = 0.0f;
    float coeff = 1.0f;


    bool running = true;
    while(running)
    {
        NodeGraphCommand command;
        while(commands.read(&command, 1))
        {
            switch(command)
            {
                case NodeGraphCommand::Quit:
                {
                    running = false;
                    break;
                }

                default:
                    break;
            }
        }

        SoundDriverIOStatus status;
        while(status_port->readStatus(&status, 1));

        if(status)
        {
            MidiEvent event;
            while(midi_in->readEvents(&event, 1))
            {
                auto msg = event.message();
                switch(msg.type())
                {
                    case MidiMessage::Type::ControlChange:
                    {
                        cout << msg.controllerNumber() << "," << msg.controllerValue() << "\n";

                        if(msg.controllerNumber() == 1)
                        {
                            freq1 = 55.0f + 880.0f * float(msg.controllerValue()) / 128.0f;
                        }

                        if(msg.controllerNumber() == 2)
                        {
                            freq2 = 55.0f + 880.0f * float(msg.controllerValue()) / 128.0f;
                        }

                        if(msg.controllerNumber() == 3)
                        {
                            coeff = float(msg.controllerValue()) / 128.0f;
                        }

                        break;
                    }

                    default:
                    {
                        cout << "other\n";
                        break;
                    }
                }
            }

            for(int i=0; i<driver->bufferSize(); i++)
            {
                float f = freq1;

                float d2 = freq2 / float(driver->sampleRate());
                f *= sin(t2 * M_PI * 2.0f) * coeff + (1.0f - coeff);
                t2 += d2;
                if(t2 >= 0.0f)
                    t2 -= 1.0f;

                float d1 = f / float(driver->sampleRate());
                buffer[i] = sin(t1 * M_PI * 2.0f);
                t1 += d1;
                if(t1 >= 0.0f)
                    t1 -= 1.0f;
            }

            audio_out->writeSamples(buffer, driver->bufferSize());
        }

        sleep_microseconds(100);
    }


    delete buffer;


    return 0;
}


NodeGraph::NodeGraph(SoundDriver* driver)
{
    if(!driver)
        return;

    m = new NodeGraphImpl;
    m_impl->driver = driver;
}


NodeGraph::~NodeGraph()
{
    if(!m_impl)
        return;

    if(m_impl->is_enabled)
        m_impl->disable();

    delete m_impl;
}


void NodeGraph::enable()
{
    if(m_impl)
        m_impl->enable();
}


void NodeGraph::disable()
{
    if(m_impl)
        m_impl->disable();
}

}//namespace r64fx