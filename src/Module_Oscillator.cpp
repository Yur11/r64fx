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
    OscillatorThreadObjectImpl(ThreadObjectIface* iface, ThreadObjectManagerImpl* manager_impl)
    : ModuleThreadObjectImpl(iface, manager_impl)
    {
        cout << "OscillatorThreadObjectImpl\n";
    }

    ~OscillatorThreadObjectImpl()
    {
        cout << "~OscillatorThreadObjectImpl\n";
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        cout << "msg: " << long(msg.value()) << "\n";
    }
};



/*======= Agents =======*/

class OscillatorDeploymentAgent : public ModuleThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* iface, ThreadObjectManagerImpl* manager_impl)
    {
        return new(std::nothrow) OscillatorThreadObjectImpl(iface, manager_impl);
    }
};

class OscillatorWithdrawalAgent : public ModuleThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl)
    {
        auto obj_impl = static_cast<OscillatorThreadObjectImpl*>(impl);
        delete obj_impl;
    }
};


/*======= Main Thread =======*/

class OscillatorThreadObjectIface : public ModuleThreadObjectIface{
    virtual ThreadObjectDeploymentAgent* newDeploymentAgent()
    {
        return new(std::nothrow) OscillatorDeploymentAgent;
    }

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent()
    {
        return new(std::nothrow) OscillatorWithdrawalAgent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
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


bool Module_Oscillator::engage()
{
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG
    m = new(std::nothrow) OscillatorThreadObjectIface;
    if(!m)
        return false;

    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        cout << "engaged!\n";
    }, m_thread_object_iface);
    return true;
}


void Module_Oscillator::disengage()
{
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        cout << "disengaged!\n";
    });
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
