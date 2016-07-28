#include "HostModel.hpp"
#include "Thread.hpp"
#include "CircularBuffer.hpp"
#include "sleep.hpp"
#include "ProcessorMessage.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

constexpr unsigned long PickDestination  = 0;
constexpr unsigned long Terminate        = 1;
constexpr unsigned long DeployMachine    = 2;
constexpr unsigned long RemoveMachine    = 3;

}//namespace

struct ProcessorThread : public Thread{
    CircularBuffer<ProcessorMessage>* m_input_buffer = nullptr;
    CircularBuffer<ProcessorMessage>* m_output_buffer = nullptr;

    bool m_this_picked = false;

public:
    ProcessorThread()
    {
        m_input_buffer  = new CircularBuffer<ProcessorMessage>(32);
        m_output_buffer = new CircularBuffer<ProcessorMessage>(32);

        run([](void* arg) -> void* {
            auto self = (ProcessorThread*)arg;
            self->exec();
            return nullptr;
        }, this);
    }


    virtual ~ProcessorThread()
    {
        if(!m_this_picked)
            pickDestination(this);
        sendMessage(Terminate);
        join();
        delete m_input_buffer;
        delete m_output_buffer;
    }


    inline void sendMessage(unsigned long type, unsigned long param)
    {
        sendMessage(ProcessorMessage(type, param));
    }


    inline void sendMessage(const ProcessorMessage &msg)
    {
        if(!m_input_buffer->write(&msg, 1))
        {
            cerr << "Failed to write message!\n";
        }
    }


    inline bool readMessage(ProcessorMessage &msg)
    {
        return m_output_buffer->read(&msg, 1) == 1;
    }


    inline void pickDestination(void* dst)
    {
        m_this_picked = (dst == this ? true : false);
        sendMessage(PickDestination, (unsigned long)dst);
    }


    inline void deployMachine(MachineModel* machine)
    {
        if(!m_this_picked)
            pickDestination(this);
        sendMessage(DeployMachine, (unsigned long)machine);
    }


    inline void removeMachine(MachineModel* machine)
    {
        if(!m_this_picked)
            pickDestination(this);
        sendMessage(RemoveMachine, (unsigned long)machine);
    }

private:
    void exec();
};


HostModel::HostModel()
{
    m_processor_thread = new ProcessorThread;
}


HostModel::~HostModel()
{
    delete m_processor_thread;
}


void HostModel::deployMachine(MachineModel* machine)
{
    m_processor_thread->deployMachine(machine);
}


void HostModel::removeMachine(MachineModel* machine)
{
    m_processor_thread->removeMachine(machine);
}


void HostModel::pickMachine(MachineModel* machine)
{
    m_processor_thread->pickDestination(machine);
}


void ProcessorThread::exec()
{
    void* message_destination = nullptr;

    bool running = true;
    while(running)
    {
        ProcessorMessage msg;
        while(m_input_buffer->read(&msg, 1) == 1)
        {
            if(msg.type == PickDestination)
            {
                message_destination = (void*) msg.param;
                continue;
            }

            if(message_destination == this)
            {
                if(msg.type == Terminate)
                {
                    running = false;
                }
            }
        }

        sleep_microseconds(500);
    }
}

}//namespace r64fx