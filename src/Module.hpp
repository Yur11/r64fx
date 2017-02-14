#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

namespace r64fx{

class Module;

enum class ModulePortType{
    Bad,
    Signal,
    Sequencer
};

class ModulePort{
    ModulePortType m_type = ModulePortType::Bad;

public:
    ModulePort(ModulePortType type) : m_type(type) {}

    ModulePort() {}

    inline ModulePortType type() const { return m_type; }
};

class ModuleSink : public ModulePort{
public:
    using ModulePort::ModulePort;
};

class ModuleSource : public ModulePort{
public:
    using ModulePort::ModulePort;
};


typedef void (*ModuleCallback)(Module* module, void* arg);

typedef void (*ModulePortCallback)(ModulePort* port, void* arg);


class Module{
    friend class ModulePrivate;
    void* m = nullptr;

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
