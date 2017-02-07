#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "ModuleFlags.hpp"

namespace r64fx{

class SoundDriver;
class SoundDriverSyncPort;


class ModuleThreadAssets{
    friend class ModuleThreadObjectImpl;
    unsigned long         count      = 0;
    SoundDriverSyncPort*  sync_port  = nullptr;
};


class ModuleThreadObjectImpl : public ThreadObjectImpl, public LinkedList<ModuleThreadObjectImpl>::Node{
    ModuleThreadObjectImpl*             m_parent    = nullptr;
    LinkedList<ModuleThreadObjectImpl>  m_children;

protected:
    unsigned long m_flags = 0;

public:
    ModuleThreadObjectImpl(ThreadObjectIface* iface, ThreadObjectManagerImpl* manager_impl);

    virtual ~ModuleThreadObjectImpl();

protected:
    ModuleThreadAssets* threadAssets() const;

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);

    virtual void runThread();

    virtual void exitThread();
};


class ModuleThreadObjectDeploymentAgent : public ThreadObjectDeploymentAgent{
    SoundDriverSyncPort* m_sync_port = nullptr;

public:
    ModuleThreadObjectDeploymentAgent();
};


class ModuleThreadObjectWithdrawalAgent : public ThreadObjectWithdrawalAgent{
};


class ModuleThreadObjectIface : public ThreadObjectIface{
    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent);

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent);
};


struct ModuleGlobal{
    static SoundDriver* soundDriver();
};

#ifndef R64FX_MODULE_PRIVATE_IMPL
extern
#endif//R64FX_MODULE_PRIVATE_IMPL
ModuleGlobal g_module;

}//namespace r64fx

#endif//R64FX_MODULE_PRIVATE_HPP
