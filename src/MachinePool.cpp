#include "MachinePool.hpp"

namespace r64fx{

MachinePool::MachinePool()
{
    startNewThread();
}


MachinePool::~MachinePool()
{
    
}


void MachinePool::deployMachine(MachineIface* machine)
{
#ifdef R64FX_DEBUG
    assert(machine != nullptr);
#endif//R64FX_DEBUG
    
    auto thread = getThread();
#ifdef R64FX_DEBUG
    assert(thread != nullptr);
#endif//R64FX_DEBUG
    
    machine->m_deployment_thread = thread;
    machine->deploymentEvent();
}


void MachinePool::withdrawMachine(MachineIface* machine)
{
#ifdef R64FX_DEBUG
    assert(machine != nullptr);
#endif//R64FX_DEBUG

    machine->withdrawalEvent();
}


void MachinePool::withdrawAllMachines()
{
    for(auto machine : machines())
    {
        withdrawMachine(machine);
    }
}


IteratorPair<LinkedList<MachineIface>::Iterator> MachinePool::machines() const
{
    return {m_machines.begin(), m_machines.end()};
}


MachinePoolThread* MachinePool::getThread() const
{
    return m_threads.first();
}


MachinePoolThread* MachinePool::startNewThread()
{
    auto thread = new MachinePoolThread;
    m_threads.append(thread);
    thread->start();
    return thread;
}

}//namespace r64fx
