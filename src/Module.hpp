#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

#include "TypeUtils.hpp"

namespace r64fx{

class Module;
class ModuleThreadHandle;


class Module{
    friend class ModulePrivate;

public:
    Module();

    virtual ~Module();

    typedef void (Callback)(Module* module, void* arg);

    virtual bool engage(
        Module::Callback* done = nullptr, void* done_arg = nullptr, ModuleThreadHandle* threads = nullptr, int nthreads = 0
    ) = 0;

    virtual void disengage(Module::Callback* done = nullptr, void* done_arg = nullptr) = 0;

    virtual bool isEngaged() = 0;

    virtual bool engagementPending() = 0;

    static void genThreads(ModuleThreadHandle** threads, int nthreads);

    static int freeThreads(ModuleThreadHandle** threads, int nthreads);
};


enum class ModulePortType : unsigned long{
    SignalSource, SignalSink
};

class ModulePort{
    R64FX_OPAQUE_HANDLE(ModulePort)

public:
    ModuleThreadHandle* thread();
};

class ModuleSource : public ModulePort{
};

class ModuleSink : public ModulePort{
};


class ModuleLink{
    ModuleSource*  m_source   = nullptr;
    ModuleSink*    m_sink     = nullptr;
    void*          m_payload  = nullptr;

public:
    ModuleLink(ModuleSource* source, ModuleSink* sink)
    : m_source(source), m_sink(sink) {}

    inline ModuleSource* source() const { return m_source; }

    inline ModuleSink* sink() const { return m_sink; }

    bool isEnabled() const;

    typedef void (Callback)(ModuleLink** links, unsigned int nlinks, void* arg);

    static void enable(ModuleLink** links, unsigned int nlinks, ModuleLink::Callback* callback, void* arg);

    static void disable(ModuleLink** links, unsigned int nlinks, ModuleLink::Callback* callback, void* arg);
};

}//namespace r64fx

#endif//R64FX_MODULE_HPP
