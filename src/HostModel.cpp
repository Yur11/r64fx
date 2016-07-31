#include "HostModel.hpp"
#include "MachineModel.hpp"
#include "MachineProcessor.hpp"
#include "Thread.hpp"
#include "Timer.hpp"
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


    inline void deployMachine(MachineProcessor* machine_processor)
    {
        if(!m_this_picked)
            pickDestination(this);
        sendMessage(DeployMachine, (unsigned long)machine_processor);
    }


    inline void removeMachine(MachineProcessor* machine_processor)
    {
        if(!m_this_picked)
            pickDestination(this);
        sendMessage(RemoveMachine, (unsigned long)machine_processor);
    }

private:
    void exec();
};


class HostModelPrivate{
    
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
    m_processor_thread->deployMachine(machine->processor());
}


void HostModel::removeMachine(MachineModel* machine)
{
    m_processor_thread->removeMachine(machine->processor());
}


void HostModel::pickMachine(MachineModel* machine)
{
    m_processor_thread->pickDestination(machine->processor());
}


void ProcessorThread::exec()
{
    ProcessorThreadContext* ctx = new ProcessorThreadContext;

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
                else if(msg.type == DeployMachine)
                {
                    auto machine_processor = (MachineProcessor*) msg.param;
                    ctx->machines.append(machine_processor);
                    machine_processor->deploy(ctx);
                }
                else if(msg.type == RemoveMachine)
                {
                    auto machine_processor = (MachineProcessor*) msg.param;
                    machine_processor->remove(ctx);
                    ctx->machines.remove(machine_processor);
                }
            }
            else if(message_destination)
            {
                auto machine_processor = (MachineProcessor*) message_destination;
                machine_processor->dispatchMessage(ctx, msg);
            }
        }

        sleep_microseconds(500);
    }

    delete ctx;
}


void HostModel::dispatchMessage(ProcessorMessage msg)
{
    
}

}//namespace r64fx