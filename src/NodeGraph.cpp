#include "NodeGraph.hpp"
#include "SoundDriver.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "sleep.hpp"
#include <iostream>
#include <cmath>


using namespace std;

namespace r64fx{

enum class NodeGraphCommand{
    Quit
};


class NodeClassImpl;

class NodeImpl : public Node, public LinkedList<NodeImpl>::Node{
    int m_size;

public:
    NodeImpl(int size)
    : m_size(size)
    {

    }

    virtual ~NodeImpl()
    {

    }

    virtual void setSize(int size)
    {
        m_size = size;
    }

    virtual int size()
    {
        return m_size;
    }
};


struct NodeClassImpl{
    LinkedList<NodeImpl> m_nodes;

    Node* newNode()
    {
        auto impl = new(std::nothrow) NodeImpl(1);
        if(!impl)
            return nullptr;
        m_nodes.append(impl);
        return impl;
    }

    void deleteNode(Node* node)
    {
        auto impl = dynamic_cast<NodeImpl*>(node);
        if(impl)
        {
            m_nodes.remove(impl);
            delete impl;
        }
    }

    virtual void process() = 0;
};


struct NodeClassImpl_AudioInput
: public NodeClass_AudioInput
, public NodeClassImpl{

    virtual void process()
    {

    }
};


struct NodeClassImpl_AudioOutput
: public NodeClass_AudioOutput
, public NodeClassImpl{
    float**  buffers  = nullptr;
    float*   ports    = nullptr;

    virtual void process()
    {

    }
};


class NodeClassImpl_MidiIO
: public NodeClass_MidiIO
, public NodeClassImpl{
    virtual void process()
    {

    }
};


class NodeClassImpl_Oscillator
: public NodeClass_Oscillator
, public NodeClassImpl{

    virtual void process()
    {

    }
};


class NodeClassImpl_Player
: public NodeClass_Player
, public NodeClassImpl{
    virtual void process()
    {

    }
};


struct NodeGraphImpl : public NodeGraph{
    SoundDriver* driver = nullptr;

    bool is_enabled = false;

    Thread thread;

    CircularBuffer<NodeGraphCommand> commands;

    LinkedList<NodeClassImpl> m_node_classes;


    NodeGraphImpl(SoundDriver* sound_driver)
    : driver(sound_driver)
    , commands(16)
    {

    }


    virtual ~NodeGraphImpl()
    {

    }


    virtual void enable()
    {
        thread.run([](void* arg) -> void*{
            auto self = (NodeGraphImpl*)arg;
            return self->process();
        }, this);

        is_enabled = true;
    }


    virtual void disable()
    {
        commands.write(NodeGraphCommand::Quit);
        thread.join();
        is_enabled = false;
    }


    void* process()
    {
        SoundDriverIOStatusPort* status_port = driver->newStatusPort();

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

            }

            sleep_microseconds(100);
        }

        return 0;
    }
};


NodeGraph* NodeGraph::newInstance(SoundDriver* sound_driver)
{
    if(!sound_driver)
        return nullptr;

    return new NodeGraphImpl(sound_driver);
}


void NodeGraph::deleteInstance(NodeGraph* graph)
{
    auto ngi = dynamic_cast<NodeGraphImpl*>(graph);
    if(ngi)
    {
        delete ngi;
    }
}

}//namespace r64fx