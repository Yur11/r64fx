#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "TimeUtils.hpp"
#include "SoundDriver.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    SoundDriver* g_SoundDriver = nullptr;
}//namespace


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


void ModuleThreadObjectImpl::runThread()
{
    
}


void ModuleThreadObjectImpl::exitThread()
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


SoundDriver* ModuleGlobal::soundDriver()
{
#ifdef R64FX_DEBUG
    assert(g_SoundDriver != nullptr);
#endif//R64FX_DEBUG
    return g_SoundDriver;
}


Module::Module()
{

}


Module::~Module()
{

}


ModuleConnection::ModuleConnection(Module* source_module, ModuleSource* source_port, Module* sink_module, ModuleSink* sink_port)
{
    
}

}//namespace r64fx
