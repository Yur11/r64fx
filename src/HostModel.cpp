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

/* GUI <-> Processor cross-thread communication protocol. */

constexpr unsigned long PickDestination  = 0;

constexpr unsigned long Terminate        = 1;
constexpr unsigned long DeployMachine    = 2;
constexpr unsigned long DetachMachine    = 3;

}//namespace


class HostModelPrivate{
    HostModel* m_model = nullptr;

    /* Cross-Thread communication buffers. */
    CircularBuffer<ProcessorMessage>* m_gui2processor = nullptr;
    CircularBuffer<ProcessorMessage>* m_processor2gui = nullptr;
    
    /* Processor thread. */
    Thread* m_thread = nullptr;

    /* GUI thread timer to read and dispatch messages from processor thread. */
    Timer* m_timer = nullptr;
    
    /* MachineProcessor to send messages to via m_gui2processor buffer. */
    MachineProcessor* m_dst_processor = nullptr;
    
    /* MachineModel to dispatch incoming messages from m_processor2gui buffer. */
    MachineModel* m_dst_model = nullptr;
    
public:
    HostModelPrivate(HostModel* model);
    
    ~HostModelPrivate();
    
    inline CircularBuffer<ProcessorMessage>* gui2processor() const
    {
        return m_gui2processor;
    }
    
    inline CircularBuffer<ProcessorMessage>* processor2gui() const
    {
        return m_processor2gui;
    }
    
    void startProcessorThread();
    
    void stopProcessorThread();
    
    void startDispatchTimer();
    
    void stopDispatchTimer();

    void sendMessage(const ProcessorMessage &msg);
    
    void sendMessage(MachineProcessor* dst, const ProcessorMessage &msg);
    
    void pickDestination(MachineProcessor* dst);

    void dispatchMessages();
    
    void deployMachine(MachineModel* machine);
    
    void detachMachine(MachineModel* machine);
};


class HostProcessorPrivate{
    /* Cross-Thread communication buffers. */
    CircularBuffer<ProcessorMessage>* m_gui2processor = nullptr;
    CircularBuffer<ProcessorMessage>* m_processor2gui = nullptr;
    
    /* MachineProcessor to dispatch incoming messages to.*/
    MachineProcessor* m_dst_processor = nullptr;
    
    /* MachineModel to send messages to. */
    MachineModel* m_dst_model = nullptr;
    
    bool m_running = true;
    
public:
    HostProcessorPrivate(CircularBuffer<ProcessorMessage>* gui2processor, CircularBuffer<ProcessorMessage>* processor2gui)
    : m_gui2processor(gui2processor)
    , m_processor2gui(processor2gui)
    {}
    
    void run();

    void sendMessage(const ProcessorMessage &msg);

    void sendMessage(MachineModel* dst, const ProcessorMessage &msg);
    
    void pickDestination(MachineModel* dst);
    
    void dispatchMessages();
};


HostModel::HostModel()
{
    m = new HostModelPrivate(this);
}


HostModel::~HostModel()
{
    if(m)
        delete m;
}


void HostModel::deployMachine(MachineModel* machine)
{
    m->deployMachine(machine);
}


HostModelPrivate::HostModelPrivate(HostModel* model)
: m_model(model)
{
    m_gui2processor = new CircularBuffer<ProcessorMessage>(32);
    m_processor2gui = new CircularBuffer<ProcessorMessage>(32);
    
    m_thread = new Thread;
    
    m_timer = new Timer;
    m_timer->setInterval(500);
    
    startDispatchTimer();
    startProcessorThread();
}
    
    
HostModelPrivate::~HostModelPrivate()
{
    stopDispatchTimer();
    stopProcessorThread();
    
    delete m_timer;
    delete m_thread;
    delete m_gui2processor;
    delete m_processor2gui;
}


void HostModelPrivate::startProcessorThread()
{
    m_thread->run([](void* arg) -> void* {
        auto m = (HostModelPrivate*) arg;
        auto p = new HostProcessorPrivate(m->gui2processor(), m->processor2gui());
        p->run();
        delete p;
        return nullptr;
    }, this);
}


void HostModelPrivate::stopProcessorThread()
{
    ProcessorMessage msg(Terminate, 0);
    sendMessage(nullptr, msg);
    m_thread->join();
}


void HostModelPrivate::startDispatchTimer()
{
    m_timer->onTimeout([](Timer* timer, void* arg){
        auto m = (HostModelPrivate*) arg;
        m->dispatchMessages();
    }, this);
    m_timer->start();
}
    
    
void HostModelPrivate::stopDispatchTimer()
{
    m_timer->stop();
}


void HostModelPrivate::sendMessage(const ProcessorMessage &msg)
{
    m_gui2processor->write(&msg, 1);
}



void HostModelPrivate::sendMessage(MachineProcessor* dst, const ProcessorMessage &msg)
{
    if(m_dst_processor != dst)
        pickDestination(dst);
    sendMessage(msg);
}


void HostModelPrivate::pickDestination(MachineProcessor* dst)
{
    ProcessorMessage msg(PickDestination, (unsigned long)dst);
    sendMessage(msg);
    m_dst_processor = dst;
}


void HostModelPrivate::dispatchMessages()
{
    ProcessorMessage msg;
    while(m_gui2processor->read(&msg, 1))
    {
        if(msg.header == PickDestination)
        {
            if(msg.header == 0)
            {
                m_dst_model = nullptr;
                m_dst_processor = nullptr;
            }
            else
            {
                m_dst_model = (MachineModel*) msg.value;
                m_dst_processor = m_dst_model->processor();
            }
        }
        else
        {
            if(m_dst_model == nullptr)
            {
                
            }
            else
            {
                m_dst_model->dispatchMessage(msg);
            }
        }
    }
}


void HostModelPrivate::deployMachine(MachineModel* machine)
{
    ProcessorMessage msg(DeployMachine, (unsigned long) machine->processor());
    sendMessage(nullptr, msg);
}


void HostModelPrivate::detachMachine(MachineModel* machine)
{
    ProcessorMessage msg(DetachMachine, (unsigned long) machine->processor());
    sendMessage(nullptr, msg);
}


void MachineModel::detach()
{
    m_parent_host->detachMachine(this);
}


void MachineModel::sendMessage(const ProcessorMessage &msg)
{
    m_parent_host->sendMessage(processor(), msg);
}


void HostProcessorPrivate::run()
{
    while(m_running)
    {
        dispatchMessages();
    }
    
    cout << "Thread Exit!\n";
}


void HostProcessorPrivate::sendMessage(const ProcessorMessage &msg)
{
    m_processor2gui->write(&msg, 1);
}


void HostProcessorPrivate::sendMessage(MachineModel* dst, const ProcessorMessage &msg)
{
    if(m_dst_model != dst)
        pickDestination(dst);
    sendMessage(msg);
}


void HostProcessorPrivate::pickDestination(MachineModel* dst)
{
    ProcessorMessage msg(PickDestination, (unsigned long)dst);
    sendMessage(msg);
    m_dst_model = dst;
}


void HostProcessorPrivate::dispatchMessages()
{
    ProcessorMessage msg;
    while(m_gui2processor->read(&msg, 1))
    {        
        if(msg.header == PickDestination)
        {
            if(msg.value == 0)
            {
                m_dst_processor = nullptr;
                m_dst_model = nullptr;
            }
            else
            {
                m_dst_processor = (MachineProcessor*) msg.value;
                m_dst_model = m_dst_processor->model();
            }
        }
        else
        {
            if(m_dst_processor == nullptr)
            {
                if(msg.header == Terminate)
                {
                    m_running = false;
                }
                else if(msg.header == DeployMachine)
                {
                    
                }
                else if(msg.header == DetachMachine)
                {
                    
                }
            }
            else
            {
                m_dst_processor->dispatchMessage(msg);
            }
        }
    }
}
    
}//namespace r64fx