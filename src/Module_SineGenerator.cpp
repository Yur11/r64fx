#include "Module_SineGenerator.hpp"
#include "ModulePrivate.hpp"
#include "SignalNode_SineGenerator.hpp"


namespace r64fx{

namespace{

struct SineGeneratorDeploymentArgs{
    SignalSource* signal_source = nullptr;
};

struct SineGeneratorWithdrawalArgs{};


/*======= Worker Thread =======*/

R64FX_DECL_MODULE_AGENTS(SineGenerator);

class SineGeneratorThreadObjectImpl : public ModuleThreadObjectImpl{
    SignalNode_SineGenerator    m_sine_generator;
    SignalNode_ScalarExtractor  m_scalar_extractor;

public:
    SineGeneratorThreadObjectImpl(SineGeneratorDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    , m_sine_generator(signalGraph()) , m_scalar_extractor(signalGraph())
    {
        signalGraph()->link(m_sine_generator.out(), m_scalar_extractor.in());
        agent->signal_source = m_scalar_extractor.out();
    }

    ~SineGeneratorThreadObjectImpl()
    {
        signalGraph()->unlink(m_scalar_extractor.in());
    }

    void storeWithdrawalArgs(SineGeneratorWithdrawalAgent* agent)
    {
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
    }
};

R64FX_DEF_MODULE_AGENTS(SineGenerator)


/*======= Main Thread =======*/

class SineGeneratorThreadObjectIface : public ModuleThreadObjectIface{
public:
    ModuleSignalSourceImpl source;

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
        { return new(std::nothrow) SineGeneratorDeploymentAgent; }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        source.signal_port = static_cast<SineGeneratorDeploymentAgent*>(agent)->signal_source;
        source.thread = thread();
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
        { return new(std::nothrow) SineGeneratorWithdrawalAgent; }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
        { delete agent; }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
    }
};

}//namespace


#define m_thread_object_iface ((SineGeneratorThreadObjectIface*)m)


Module_SineGenerator::Module_SineGenerator()
{
    m = new(std::nothrow) SineGeneratorThreadObjectIface;
}


Module_SineGenerator::~Module_SineGenerator()
{
    R64FX_DEBUG_ASSERT(!isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
    delete m_thread_object_iface;
}


bool Module_SineGenerator::engage(Module::Callback* done, void* done_arg, ModuleThreadHandle* threads, int nthreads)
{
    R64FX_DEBUG_ASSERT(!isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
    ModulePrivate::deploy(m_thread_object_iface, nullptr, done, done_arg, this);
    return true;
}


void Module_SineGenerator::disengage(Module::Callback* done, void* done_arg)
{
    R64FX_DEBUG_ASSERT(isEngaged());
    R64FX_DEBUG_ASSERT(!engagementPending());
    ModulePrivate::withdraw(m_thread_object_iface, done, done_arg, this);
}


bool Module_SineGenerator::isEngaged()
{
    return m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_SineGenerator::engagementPending()
{
    return m_thread_object_iface->isPending();
}


ModuleSource* Module_SineGenerator::source() const
{
    return m_thread_object_iface->source.modulePort();
}

}//namespace r64fx
