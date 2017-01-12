#include "Module.hpp"
#define R64FX_MODULE_PRIVATE_IMPL
#include "ModulePrivate.hpp"
#include "TimeUtils.hpp"
#include "SoundDriver.hpp"


namespace r64fx{

namespace{
    ModuleGlobal g;

    SoundDriver* g_SoundDriver = nullptr;

    long int g_module_exec_agent_count = 0;
}//namespace


struct ModuleThreadObjectExecAgent : public ThreadObjectExecAgent{
    bool m_running;
    SoundDriverIOStatusPort* m_status_port;

    ModuleThreadObjectExecAgent()
    {
        if(g_module_exec_agent_count == 0)
        {
            g_SoundDriver = SoundDriver::newInstance(SoundDriver::Type::Stub);
        }
        g_module_exec_agent_count++;
        m_status_port = g_SoundDriver->newStatusPort();
    }

    virtual ~ModuleThreadObjectExecAgent()
    {
        g_SoundDriver->deletePort(m_status_port);
        g_module_exec_agent_count--;
        if(g_module_exec_agent_count == 0)
        {
            SoundDriver::deleteInstance(g_SoundDriver);
        }
    }

    virtual void exec()
    {
        m_running = true;
        while(m_running)
        {
            SoundDriverIOStatus status;
            if(m_status_port->readStatus(&status, 1))
            {
                //Do Stuff!
            }

            readMessagesFromIface();
            sleep_nanoseconds(1500 * 1000);
        }
    }

    virtual void terminate()
    {
        m_running = false;
    }
};


void ModuleThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


// void ModuleThreadObjectIface::messageFromImplRecieved(const ThreadObjectMessage &msg)
// {
//     
// }


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
