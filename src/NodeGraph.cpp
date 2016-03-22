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

    float freq = 440.0f;
    float t = 0.0f;


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
            float d = freq / float(driver->sampleRate());
            for(int i=0; i<driver->bufferSize(); i++)
            {
                buffer[i] = sin(t * M_PI * 2.0f);
                t += d;
                if(t >= 0.0f)
                    t -= 1.0f;

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