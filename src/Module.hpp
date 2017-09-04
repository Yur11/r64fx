#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

namespace r64fx{

class Module;

class ModulePort{
    friend class ModuleSink;
    friend class ModuleSource;
    friend class ModulePrivate;

    Module*        m_parent   = nullptr;
    void*          m_payload  = nullptr;
    unsigned long  m_flags    = 0;

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


typedef void (*ModuleCallback)(Module* module, void* arg);

typedef void (*ModulePortCallback)(ModulePort* port, void* arg);


class Module{
    friend class ModulePrivate;
    int m_thread_id = 0;

public:
    Module();

    virtual ~Module();

    virtual bool engage(ModuleCallback done = nullptr, void* done_arg = nullptr) = 0;

    virtual void disengage(ModuleCallback done = nullptr, void* done_arg = nullptr) = 0;

    virtual bool isEngaged() = 0;

    virtual bool engagementPending() = 0;

    void changeThread(int thread_id);

    int threadId() const;
};


class ModuleConnection{
    ModuleSource*  m_source_port  = nullptr;
    ModuleSink*    m_sink_port    = nullptr;
    void*          m_payload      = nullptr;

public:
    ModuleConnection(ModuleSource* source_port, ModuleSink* sink_port);

    bool enabled();

    typedef void (Callback)(ModuleConnection* connections, int nconnections, void* arg);

    static void enableBulk(ModuleConnection* connections, int nconnections, ModuleConnection::Callback* callback, void* arg);

    static void disableBulk(ModuleConnection* connections, int nconnections, ModuleConnection::Callback* callback, void* arg);
};

}//namespace r64fx

#endif//R64FX_MODULE_HPP
