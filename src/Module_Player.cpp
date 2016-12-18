#include "Module_Player.hpp"
#include "ModulePrivate.hpp"

namespace r64fx{

class PlayerThreadObjectIface : public ModuleThreadObjectIface{
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg);

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent();
};


class PlayerThreadObjectImpl : public ModuleThreadObjectImpl{
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);

public:
    using ModuleThreadObjectImpl::ModuleThreadObjectImpl;
};


class PlayerThreadObjectDeploymentAgent : public ModuleThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface);
};


Module_Player::Module_Player()
{
    
}


Module_Player::~Module_Player()
{
    
}


void PlayerThreadObjectIface::messageFromImplRecieved(const ThreadObjectMessage &msg)
{
    
}


ThreadObjectDeploymentAgent* PlayerThreadObjectIface::newDeploymentAgent()
{
    return new PlayerThreadObjectDeploymentAgent;
}


void PlayerThreadObjectImpl::messageFromIfaceRecieved(const ThreadObjectMessage &msg)
{
    
}


ThreadObjectImpl* PlayerThreadObjectDeploymentAgent::deployImpl(ThreadObjectIface* public_iface)
{
    return new PlayerThreadObjectImpl(public_iface);
}

}//namespace r64fx
