#include "MachinePool.hpp"

namespace r64fx{

MachinePool::MachinePool()
{
    startNewThread();
}


MachinePool::~MachinePool()
{
    
}
    

LinkedList<Machine>::Iterator MachinePool::begin() const
{
    return m_machines.begin();
}


LinkedList<Machine>::Iterator MachinePool::end() const
{
    return m_machines.end();
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
