#include "MachineIface.hpp"
#include "MachineFlags.hpp"
#include "MachinePool.hpp"

#include <iostream>
using namespace std;

namespace r64fx{


MachineIface::MachineIface()
{

}


MachineIface::~MachineIface()
{
    if(isDeployed())
    {
        withdraw();
    }
}


MachinePool* MachineIface::parentPool() const
{
    return m_parent_pool;
}


bool MachineIface::isDeployed() const
{
    return m_deployed_impl != nullptr;
}


void MachineIface::withdraw()
{
    if(m_parent_pool && m_deployed_impl)
    {
        m_parent_pool->withdrawMachine(this);
        m_parent_pool = nullptr;
        m_deployed_impl = nullptr;
    }
}


inline void MachineIface::sendMessagesToImpl(MachineMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(isDeployed());
#endif//R64FX_DEBUG
    m_deployment_thread->sendMessagesToImpl(m_deployed_impl, msgs, nmsgs);
}


void MachineIface::implDeployed(MachineImpl* impl)
{
#ifdef R64FX_DEBUG
    assert(impl != nullptr);
#endif//R64FX_DEBUG
    m_deployed_impl = impl;
}

}//namespace r64fx
