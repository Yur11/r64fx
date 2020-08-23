#ifndef R64FX_ENGINE_HPP
#define R64FX_ENGINE_HPP

#include <vector>
#include <string>

namespace r64fx{

/*=== Base Engine Classes ===*/

struct EngineObject{
    void* impl = nullptr;
    unsigned long bits = 0;
    std::string name = "";

    inline unsigned long type() const { return bits; }

    inline bool isPort() const { return bits & 3; }

    inline bool isModule() const { return bits & 0xF8; }
};


template<typename T> constexpr unsigned long EngineObjectType(T* t = nullptr)
    { return 0; }

#define R64FX_ENGINE_OBJECT_TYPE(Name, Id) class Name; \
    template<> constexpr unsigned long EngineObjectType<Name>(Name*) { return Id; }

R64FX_ENGINE_OBJECT_TYPE( ModulePort,          1 )
R64FX_ENGINE_OBJECT_TYPE( ModuleSink,          2 )
R64FX_ENGINE_OBJECT_TYPE( ModuleSource,        3 )

R64FX_ENGINE_OBJECT_TYPE( Module,              4 )
R64FX_ENGINE_OBJECT_TYPE( Module_SoundDriver,  5 )


struct Module : public EngineObject{
    std::vector<EngineObject*> objects;

    Module()
        { bits = EngineObjectType(this); }
};


/*=== Module Ports ===*/

struct ModulePort : public EngineObject{
    ModulePort()
        { bits = EngineObjectType(this); }

    unsigned long nchannels = 1;
};

struct ModuleSource : public ModulePort{
    ModuleSource()
        { bits = EngineObjectType(this); }
};

struct ModuleSink : public ModulePort{
    /* Only one port can be connected to a sink at a time. */
    ModulePort* connected_source = nullptr;

    ModuleSink()
        { bits = EngineObjectType(this); }
};


/*=== Modules ===*/

struct Module_SoundDriver : Module{
    Module_SoundDriver()
        { bits = EngineObjectType(this); }
};


/*=== Engine Update Infrastructure ===*/

struct EngineUpdate{
    enum class Verb{
        None, AddObject, RemoveObject, ReplaceObject, AlterObject, RelinkPorts
    };

    typedef unsigned long Noun;

    Verb verb = Verb::None;
    Noun noun[3] = {0, 0, 0};

    EngineUpdate(Verb verb, Noun first = 0, Noun second = 0, Noun third = 0)
        : verb(verb), noun{first, second, third} {}
};


inline EngineUpdate AddObject(Module* parent, EngineObject* child)
{
    return {EngineUpdate::Verb::AddObject, (EngineUpdate::Noun)parent, (EngineUpdate::Noun)child};
}

inline EngineUpdate RemoveObject(Module* parent, EngineObject* child)
{
    return {EngineUpdate::Verb::RemoveObject, (EngineUpdate::Noun)parent, (EngineUpdate::Noun)child};
}

inline EngineUpdate ReplaceObject(Module* module, EngineObject* old_object, EngineObject* new_object)
{
    return {EngineUpdate::Verb::ReplaceObject,
        (EngineUpdate::Noun)module, (EngineUpdate::Noun)old_object, (EngineUpdate::Noun)new_object};
}

inline EngineUpdate AlterObject(EngineObject* object, bool enabled)
{
    return {EngineUpdate::Verb::AlterObject, (EngineUpdate::Noun)object, (EngineUpdate::Noun)enabled};
}

inline EngineUpdate RelinkPorts(ModuleSink* sink, ModulePort* port = nullptr)
{
    return {EngineUpdate::Verb::RelinkPorts, (EngineUpdate::Noun)sink, (EngineUpdate::Noun)port};
}


class Engine{
    Engine();
    Engine(const Engine&) {}

public:
    static Engine* singletonInstance();

    bool update(
            EngineUpdate* transaction, unsigned long size,
            void (*done)(void* arg, EngineUpdate* transaction, unsigned long size) = nullptr,
            void* arg = nullptr
    );

    inline bool update(std::vector<EngineUpdate> &transaction)
        {  return update(transaction.data(), transaction.size()); }

};

}//namespace r64fx

#endif//R64FX_ENGINE_HPP