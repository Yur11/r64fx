#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"

namespace r64fx{

class SoundDriver;

class ModuleThreadObjectIface : public ThreadObjectIface{
    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent);

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent);
/*
    virtual ThreadObjectExecAgent* newExecAgent();

    virtual void deleteExecAgent(ThreadObjectExecAgent* agent);*/
};

#define R64FX_MODULE_AGENTS(name)\
    virtual ThreadObjectDeploymentAgent* newDeploymentAgent()\
    {\
        return new name##DeploymentAgent;\
    }\
\
    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent()\
    {\
        return new name##WithdrawalAgent;\
    }\


class ModuleThreadObjectImpl : public ThreadObjectImpl, public LinkedList<ModuleThreadObjectImpl>::Node{
    ModuleThreadObjectImpl* m_parent = nullptr;
    LinkedList<ModuleThreadObjectImpl> m_children;

public:
//     ModuleThreadObjectImpl(ModuleThreadObjectIface* iface, ModuleThreadObjectImpl* parent);
    using ThreadObjectImpl::ThreadObjectImpl;

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);

    virtual void runThread();

    virtual void exitThread();
};


class ModuleThreadObjectDeploymentAgent : public ThreadObjectDeploymentAgent{
};


class ModuleThreadObjectWithdrawalAgent : public ThreadObjectWithdrawalAgent{
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
