#include "Module_SoundDriver.hpp"
#include "ModulePrivate.hpp"

namespace r64fx{

class SoundDriverThreadObjectIface : public ModuleThreadObjectIface{
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg);

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent();

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent();
};


class SoundDriverThreadObjectImpl : public ModuleThreadObjectImpl{
public:
    using ModuleThreadObjectImpl::ModuleThreadObjectImpl;

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);
};


class SoundDriverDeploymentAgent : public ModuleThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface);
};


class SoundDriverWithdrawalAgent : public ModuleThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl);
};


Module_SoundDriver::Module_SoundDriver()
{
    
}


Module_SoundDriver::~Module_SoundDriver()
{
    
}


void SoundDriverThreadObjectIface::messageFromImplRecieved(const ThreadObjectMessage &msg)
{
    
}


void SoundDriverThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


ThreadObjectDeploymentAgent* SoundDriverThreadObjectIface::newDeploymentAgent()
{
    return new SoundDriverDeploymentAgent;
}


ThreadObjectWithdrawalAgent* SoundDriverThreadObjectIface::newWithdrawalAgent()
{
    return new SoundDriverWithdrawalAgent;
}


ThreadObjectImpl* SoundDriverDeploymentAgent::deployImpl(ThreadObjectIface* public_iface)
{
    return new SoundDriverThreadObjectImpl(public_iface);
}


void SoundDriverWithdrawalAgent::withdrawImpl(ThreadObjectImpl* impl)
{
    delete impl;
}

}//namespace r64fx
