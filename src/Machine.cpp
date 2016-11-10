#include "Machine.hpp"
#include "MachineFlags.hpp"
#include "MachineImpl.hpp"
#include "MachinePool.hpp"
#include "MachinePoolThreadImpl.hpp"
#include "MachinePortImpl.hpp"
#include "MachineConnectionImpl.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

    
Machine::Machine(MachinePool* parent_pool, MachineDeploymentFun deploy, MachineWithdrawalFun withdraw)
: m_parent_pool(parent_pool)
, m_deploy(deploy)
, m_withdraw(withdraw)
{
#ifdef R64FX_DEBUG
    assert(parent_pool != nullptr);
    assert(deploy != nullptr);
    assert(withdraw != nullptr);
#endif//R64FX_DEBUG
}

    
Machine::~Machine()
{
    
}


MachinePool* Machine::parentPool() const
{
    return m_parent_pool;
}


void Machine::deploy()
{
#ifdef R64FX_DEBUG
    assert(m_parent_pool != nullptr);
#endif//R64FX_DEBUG
    
    auto thread = m_parent_pool->getThread();
#ifdef R64FX_DEBUG
    assert(thread != nullptr);
#endif//R64FX_DEBUG
    
    thread->deployMachine(m_deploy, this);
    m_deployment_thread = thread;
}


void Machine::withdraw()
{
#ifdef R64FX_DEBUG
    assert(m_parent_pool != nullptr);
#endif//R64FX_DEBUG    
}


bool Machine::isDeployed() const
{
    return m_deployed_impl != nullptr;
}


void Machine::implDeployed(MachineImpl* impl)
{
#ifdef R64FX_DEBUG
    assert(impl != nullptr);
#endif//R64FX_DEBUG
    m_deployed_impl = impl;
}

}//namespace r64fx
