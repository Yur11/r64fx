#include "Module_Oscillator.hpp"
#include "ModulePrivate.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "SoundDriver.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
/*======= Worker Thread =======*/

class OscillatorThreadObjectImpl : public ModuleThreadObjectImpl{
public:
    OscillatorThreadObjectImpl(R64FX_DEF_MODULE_IMPL_ARGS)
    : ModuleThreadObjectImpl(R64FX_MODULE_IMPL_ARGS)
    {
    }

    ~OscillatorThreadObjectImpl()
    {
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        cout << "msg: " << long(msg.value()) << "\n";
    }
};



/*======= Agents =======*/

class OscillatorDeploymentAgent : public ModuleDeploymentAgent{
    virtual ModuleThreadObjectImpl* deployModuleImpl(HeapAllocator* ha, R64FX_DEF_MODULE_IMPL_ARGS) override final
    {
        return ha->allocObj<OscillatorThreadObjectImpl>(R64FX_MODULE_IMPL_ARGS);
    }
};

class OscillatorWithdrawalAgent : public ModuleWithdrawalAgent{
    virtual void withdrawModuleImpl(HeapAllocator* ha, ModuleThreadObjectImpl* impl) override final
    {
        auto osc_impl = static_cast<OscillatorThreadObjectImpl*>(impl);
        ha->freeObj(osc_impl);
    }
};


/*======= Main Thread =======*/

class OscillatorThreadObjectIface : public ModuleThreadObjectIface{
public:
    ModuleCallback  done      = nullptr;
    void*           done_arg  = nullptr;

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        return new OscillatorDeploymentAgent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new OscillatorWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
        delete agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        
    }
};

#define m_thread_object_iface ((OscillatorThreadObjectIface*)m)


Module_Oscillator::Module_Oscillator()
{

}


Module_Oscillator::~Module_Oscillator()
{

}


bool Module_Oscillator::engage(ModuleCallback done, void* done_arg)
{
    if(!m)
        m = new(std::nothrow) OscillatorThreadObjectIface;
    if(!m)
        return false;
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG

    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto osc_iface = static_cast<OscillatorThreadObjectIface*>(iface);
        if(osc_iface->done)
        {
            osc_iface->done((Module*)arg, osc_iface->done_arg);
        }
    }, this);
    return true;
}


void Module_Oscillator::disengage(ModuleCallback done, void* done_arg)
{
    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        auto osc_iface = static_cast<OscillatorThreadObjectIface*>(iface);
        if(osc_iface->done)
        {
            osc_iface->done((Module*)arg, osc_iface->done_arg);
        }
    }, this);
}


bool Module_Oscillator::isEngaged()
{
    return m != nullptr && m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_Oscillator::engagementPending()
{
    return m_thread_object_iface->isPending();
}

}//namespace r64fx
