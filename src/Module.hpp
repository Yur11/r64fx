#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

namespace r64fx{

class Module;
class ModuleThreadHandle;

class ModulePort{
    friend class ModuleSink;
    friend class ModuleSource;
    friend class ModulePrivate;

    unsigned long        m_flags          = 0;
    Module*              m_parent         = nullptr;
    void*                m_payload        = nullptr;
    ModuleThreadHandle*  m_thread_handle  = nullptr;

public:
    ModulePort();

    inline Module* parent() const { return m_parent; };

    inline bool isSink() const { return !isSource(); }

    bool isSource() const;
};

class ModuleSink : public ModulePort{
public:
    ModuleSink();
};

class ModuleSource : public ModulePort{
public:
    ModuleSource();
};


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


class ModuleLink{
    ModuleSource*  m_source   = nullptr;
    ModuleSink*    m_sink     = nullptr;
    void*          m_payload  = nullptr;

public:
    ModuleLink(ModuleSource* source, ModuleSink* sink)
    : m_source(source), m_sink(sink) {}

    inline ModuleSource* source() const { return m_source; }

    inline ModuleSink* sink() const { return m_sink; }

    bool enabled();

    typedef void (Callback)(ModuleLink* links, int nlinks, void* arg);

    static void enable(ModuleLink* links, int nlinks, ModuleLink::Callback* callback, void* arg);

    static void disable(ModuleLink* links, int nlinks, ModuleLink::Callback* callback, void* arg);
};

}//namespace r64fx

#endif//R64FX_MODULE_HPP
