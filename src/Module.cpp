#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "TimeUtils.hpp"


namespace r64fx{

namespace{ ModuleGlobal g; }//namespace


ThreadObjectImpl* ModuleThreadObjectDeploymentAgent::deployImpl(ThreadObjectIface* public_iface)
{
    auto impl = new ModuleThreadObjectImpl(public_iface);
    return impl;
}


void ModuleThreadObjectWithdrawalAgent::withdrawImpl(ThreadObjectImpl* impl)
{
    delete impl;
}


void ModuleThreadObjectExecAgent::exec()
{
    m_running = true;
    while(m_running)
    {
        readMessagesFromIface();
        sleep_nanoseconds(1500 * 1000);
    }
}


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


void ModuleThreadObjectExecAgent::terminate()
{
    
}


void ModuleThreadObjectIface::messageFromImplRecieved(const ThreadObjectMessage &msg)
{
    
}


void ModuleThreadObjectIface::deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
{
    delete agent;
}


void ModuleThreadObjectIface::deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
{
    delete agent;
}


ThreadObjectExecAgent* ModuleThreadObjectIface::newExecAgent()
{
    return new ModuleThreadObjectExecAgent;
}


void ModuleThreadObjectIface::deleteExecAgent(ThreadObjectExecAgent* agent)
{
    delete agent;
}


inline void ModulePrivate::setModulePayload(Module* module, void* payload)
{
    module->m = payload;
}


inline void* ModulePrivate::getModulePayload(Module* module)
{
    return module->m;
}


Module::Module()
{
    
}


Module::~Module()
{
    
}

}//namespace r64fx
