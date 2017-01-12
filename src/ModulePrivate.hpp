#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"

namespace r64fx{

class SoundDriver;

class ModuleThreadObjectIface : public ThreadObjectIface{
//     virtual void messageFromImplRecieved(const ThreadObjectMessage &msg);

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent);

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent);

    virtual ThreadObjectExecAgent* newExecAgent();

    virtual void deleteExecAgent(ThreadObjectExecAgent* agent);
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


class ModuleThreadObjectImpl : public ThreadObjectImpl{
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);

public:
    using ThreadObjectImpl::ThreadObjectImpl;
};


class ModuleThreadObjectDeploymentAgent : public ThreadObjectDeploymentAgent{
//     virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface);
};


class ModuleThreadObjectWithdrawalAgent : public ThreadObjectWithdrawalAgent{
//     virtual void withdrawImpl(ThreadObjectImpl* impl);
};


struct ModuleGlobal{
    ModuleThreadObjectIface* root = nullptr;

    static SoundDriver* soundDriver();
};

#ifndef R64FX_MODULE_PRIVATE_IMPL
extern
#endif//R64FX_MODULE_PRIVATE_IMPL
ModuleGlobal g_module;

}//namespace r64fx

#endif//R64FX_MODULE_PRIVATE_HPP
