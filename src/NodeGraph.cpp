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


struct NodePortImpl{
    string  m_name   = "";
    float*  m_buffer = nullptr;
    int     m_size   = 0;

    void resize(int new_size, float default_value = 0.0f)
    {
        if(new_size == m_size)
            return;

        if(new_size > 0)
        {
            auto new_buffer = new(std::nothrow) float[new_size];
            if(!new_buffer)
            {
                cerr << "Failed to allocate port buffer!\n";
                return;
            }

            if(m_buffer)
            {
                int copy_size = min(m_size, new_size);
                for(int i=0; i<copy_size; i++)
                {
                    new_buffer[i] = m_buffer[i];
                }

                delete m_buffer;
            }
            else
            {
                for(int i=0; i<new_size; i++)
                {
                    new_buffer[i] = default_value;
                }
            }

            m_buffer = new_buffer;
            m_size = new_size;
        }
        else
        {
            if(m_buffer)
            {
                delete m_buffer;
                m_buffer = nullptr;
                m_size = 0;
            }
        }
    }
};


struct NodeInputImpl
: public NodeInput
, public NodePortImpl{
    virtual std::string name()
    {
        return m_name;
    }

    virtual bool isOutput()
    {
        return false;
    }
};


struct NodeOutputImpl
: public NodeOutput
, public NodePortImpl{
    virtual std::string name()
    {
        return m_name;
    }

    virtual bool isOutput()
    {
        return true;
    }
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


class Nodes : public LinkedList<NodeImpl>{
public:
    int totalSize() const
    {
        int num = 0;
        for(auto node : *this)
        {
            num += node->size();
        }
        return num;
    }
};


struct NodeClassImpl : public LinkedList<NodeClassImpl>::Node{
    Nodes m_nodes;

    virtual void prepare() = 0;

    virtual void process() = 0;

    virtual void updatePorts() = 0;
};


struct NodeClassImpl_AudioInput
: public NodeClass_AudioInput
, public NodeClassImpl{

    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {

    }
};


class NodeClassImpl_AudioOutput
: public NodeClass_AudioOutput
, public NodeClassImpl{

    NodeInputImpl m_node_input;

public:
    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {
        m_node_input.resize(m_nodes.totalSize());
    }
};


class NodeClassImpl_MidiInput
: public NodeClass_MidiInput
, public NodeClassImpl{

    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {

    }
};


class NodeClassImpl_MidiOutput
: public NodeClass_MidiOutput
, public NodeClassImpl{

    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {

    }
};


class NodeClassImpl_Oscillator
: public NodeClass_Oscillator
, public NodeClassImpl{

    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {

    }
};


class NodeClassImpl_Player
: public NodeClass_Player
, public NodeClassImpl{

    virtual void prepare()
    {

    }

    virtual void process()
    {

    }

    virtual void updatePorts()
    {

    }
};


struct NodeLinkImpl : public NodeLink, public LinkedList<NodeLinkImpl>::Node{
    NodeLink::Type m_type = NodeLink::Type::Mix;

    virtual Type type()
    {
        return m_type;
    }

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

    LinkedList<NodeLinkImpl> m_node_links;

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


    virtual Node* newNode(NodeClass* node_class, int size)
    {
        return nullptr;
    }


    virtual void deleteNode(Node* node)
    {

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

            for(auto node_class : m_node_classes)
            {
                node_class->updatePorts();
                node_class->prepare();
            }

            SoundDriverIOStatus status;
            while(status_port->readStatus(&status, 1));

            if(status)
            {
                for(int i=0; i<driver->bufferSize(); i++)
                {
                    for(auto node_class : m_node_classes)
                    {
                        node_class->process();
                    }

                    for(auto node_link : m_node_links)
                    {
                        node_link->process();
                    }
                }
            }
            else
            {
                sleep_microseconds(100);
            }
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