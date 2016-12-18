#ifndef R64FX_MODULE_PRIVATE_HPP
#define R64FX_MODULE_PRIVATE_HPP

#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"

namespace r64fx{

class ModuleThreadObjectDeploymentAgent : public ThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface);
};


class ModuleThreadObjectWithdrawalAgent : public ThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl);
};


class ModuleThreadObjectExecAgent : public ThreadObjectExecAgent{
    bool m_running = true;

    virtual void exec();

    virtual void terminate();
};


class ModuleThreadObjectImpl : public ThreadObjectImpl{
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg);

public:
    using ThreadObjectImpl::ThreadObjectImpl;
};


class ModuleThreadObjectIface : public ThreadObjectIface{
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg);

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent();

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent);

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent();

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent);

    virtual ThreadObjectExecAgent* newExecAgent();

    virtual void deleteExecAgent(ThreadObjectExecAgent* agent);
};


struct ModuleGlobal{
    ModuleThreadObjectIface* root = nullptr;
};

#ifndef R64FX_MODULE_PRIVATE_IMPL
extern
#endif//R64FX_MODULE_PRIVATE_IMPL
ModuleGlobal g_module;


class ModulePrivate{
public:
    static void setModulePayload(Module* module, void* payload);

    static void* getModulePayload(Module* module);
};

}//namespace r64fx

#endif//R64FX_MODULE_PRIVATE_HPP
