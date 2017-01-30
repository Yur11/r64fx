#include "Module_Oscillator.hpp"
#include "ModulePrivate.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"
#include "SoundDriver.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

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


class OscillatorDeploymentAgent : public ModuleThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface, ThreadObjectManagerImpl* manager_impl)
    {
        return new OscillatorThreadObjectImpl(public_iface, manager_impl);
    }
};


class OscillatorWithdrawalAgent : public ModuleThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl)
    {
        auto osc_impl = (OscillatorThreadObjectImpl*) impl;
        delete osc_impl;
    }
};


struct OscillatorThreadObjectIface : public ModuleThreadObjectIface{
    R64FX_MODULE_AGENTS(Oscillator)

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
    {
        
    }

    inline void engaged()
    {
        cout << "engaged!\n";
        auto sd = ModuleGlobal::soundDriver();
        auto port = sd->newAudioOutput("out");
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
        disengage();
        while(m_thread_object_iface->isPending())
        {
            Timer::runTimers();
            sleep_nanoseconds(1500 * 1000);
        }
    }
}


void Module_Oscillator::engage()
{
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto otobi = (OscillatorThreadObjectIface*) arg;
        otobi->engaged();
    }, m_thread_object_iface);
}


void Module_Oscillator::disengage()
{
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        cout << "disengaged!\n";
    });
}


bool Module_Oscillator::isEngaged()
{
    return m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}

}//namespace r64fx
