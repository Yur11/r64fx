#include "Module_Oscillator.hpp"
#include "ModulePrivate.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

class OscillatorThreadObjectImpl : public ModuleThreadObjectImpl{
public:
    using ModuleThreadObjectImpl::ModuleThreadObjectImpl;
};


class OscillatorDeploymentAgent : public ModuleThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface)
    {
        cout << "Osc Deploy!\n";
        return new OscillatorThreadObjectImpl(public_iface);
    }
};


class OscillatorWithdrawalAgent : public ModuleThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl)
    {
        auto osc_impl = (OscillatorThreadObjectImpl*) impl;
        delete osc_impl;
        cout << "Osc Withdraw!\n";
    }
};


struct OscillatorThreadObjectIface : public ModuleThreadObjectIface{
    R64FX_MODULE_AGENTS(Oscillator)

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
    {
        
    }
};

#define m_thread_object_iface ((OscillatorThreadObjectIface*)m)


Module_Oscillator::Module_Oscillator()
{
    m = new OscillatorThreadObjectIface;
}


Module_Oscillator::~Module_Oscillator()
{
    if(isEngaged())
    {
        
    }
}


void Module_Oscillator::engage()
{
    m_thread_object_iface->deploy(nullptr);
}


void Module_Oscillator::disengage()
{
    m_thread_object_iface->withdraw();
}


bool Module_Oscillator::isEngaged()
{
    return m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}

}//namespace r64fx
