#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

namespace r64fx{

class Module;

class ModulePort{
    friend class ModuleSink;
    friend class ModuleSource;
    friend class ModulePrivate;

    unsigned long  m_flags    = 0;
    void*          m_payload  = nullptr;

public:
    ModulePort();

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


typedef void (*ModuleCallback)(Module* module, void* arg);

typedef void (*ModulePortCallback)(ModulePort* port, void* arg);


class Module{
    friend class ModulePrivate;

public:
    Module();

    virtual ~Module();

    virtual bool engage(ModuleCallback done = nullptr, void* done_arg = nullptr) = 0;

    virtual void disengage(ModuleCallback done = nullptr, void* done_arg = nullptr) = 0;

    virtual bool isEngaged() = 0;

    virtual bool engagementPending() = 0;
};


class ModuleConnection{
    Module*        m_source_module  = nullptr;
    ModuleSource*  m_source_port    = nullptr;
    Module*        m_sink_module    = nullptr;
    ModuleSink*    m_sink_port      = nullptr;

public:
    ModuleConnection(Module* source_module, ModuleSource* source_port, Module* sink_module, ModuleSink* sink_port);
};

}//namespace r64fx

#endif//R64FX_MODULE_HPP
