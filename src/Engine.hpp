#ifndef R64FX_ENGINE_HPP
#define R64FX_ENGINE_HPP

#include <vector>
#include <string>

#include "Debug.hpp"

namespace r64fx{

/*=== Base Engine Classes ===*/

struct EngineObject{
    void* impl = nullptr;

    unsigned short  type    = 0;
    EngineObject*   parent  = nullptr;
    std::string     name    = "";

    inline bool isPort() const
        { return type <= 3; }

    inline bool isModule() const
        { return type >= 4; }
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
R64FX_ENGINE_OBJECT_TYPE( Module_A,            6 )
R64FX_ENGINE_OBJECT_TYPE( Module_B,            7 )
R64FX_ENGINE_OBJECT_TYPE( Module_C,            8 )


struct Module : public EngineObject{
    std::vector<EngineObject*> objects;

    Module()
        { type = EngineObjectType(this); }

    inline void addObject(EngineObject* new_object)
    {
        R64FX_DEBUG_ASSERT(new_object != nullptr);
        R64FX_DEBUG_ASSERT(new_object->parent == nullptr);
#       ifdef R64FX_DEBUG
        for(auto object : objects)
            if(object == new_object)
                R64FX_DEBUG_ABORT("Object already added!");
#       endif//R64FX_DEBUG
        objects.push_back(new_object);
        new_object->parent = this;
    }
};


/*=== Module Ports ===*/

struct ModulePort : public EngineObject{
    unsigned long nchannels = 0;

    ModulePort()
        { type = EngineObjectType(this); }
};

struct ModuleSource : public ModulePort{
    ModuleSource()
        { type = EngineObjectType(this); }
};

struct ModuleSink : public ModulePort{
    /* Only one source can be connected to a sink at a time. */
    ModuleSource* connected_source = nullptr;

    ModuleSink()
        { type = EngineObjectType(this); }
};


/*=== Modules ===*/

struct Module_SoundDriver : public Module{
    Module_SoundDriver()
        { type = EngineObjectType(this); }
};


struct Module_A : public Module{
    Module_A()
    {
        type = EngineObjectType(this);

        auto source = new ModuleSource;
        source->name = "sequence";
        addObject(source);

        auto sink = new ModuleSink;
        sink->name = "sequence";
        addObject(sink);
    }
};

struct Module_B : public Module{
    Module_B()
    {
        type = EngineObjectType(this);

        auto source = new ModuleSource;
        source->name = "sequence";
        addObject(source);

        auto sink = new ModuleSink;
        sink->name = "sequence";
        addObject(sink);
    }
};

struct Module_C : public Module{
    Module_C()
    {
        type = EngineObjectType(this);

        auto source = new ModuleSource;
        source->name = "sequence";
        addObject(source);

        auto sink = new ModuleSink;
        sink->name = "sequence";
        addObject(sink);
    }
};


/*=== Engine Update Infrastructure ===*/

struct EngineUpdate{
    enum class Verb{
        None, ReplaceRoot, AddObject, RemoveObject, ReplaceObject, AlterObject, RelinkPorts
    };

    typedef unsigned long Noun;

    Verb verb = Verb::None;
    Noun noun[3] = {0, 0, 0};

    EngineUpdate(Verb verb, Noun first = 0, Noun second = 0, Noun third = 0)
        : verb(verb), noun{first, second, third} {}
};

inline const EngineUpdate ReplaceRoot(Module* module)
{
    return {EngineUpdate::Verb::ReplaceRoot, (EngineUpdate::Noun)module};
}

inline const EngineUpdate AddObject(Module* parent, EngineObject* child)
{
    return {EngineUpdate::Verb::AddObject, (EngineUpdate::Noun)parent, (EngineUpdate::Noun)child};
}

inline const EngineUpdate RemoveObject(Module* parent, EngineObject* child)
{
    return {EngineUpdate::Verb::RemoveObject, (EngineUpdate::Noun)parent, (EngineUpdate::Noun)child};
}

inline const EngineUpdate ReplaceObject(Module* module, EngineObject* old_object, EngineObject* new_object)
{
    return {EngineUpdate::Verb::ReplaceObject,
        (EngineUpdate::Noun)module, (EngineUpdate::Noun)old_object, (EngineUpdate::Noun)new_object};
}

inline const EngineUpdate AlterObject(EngineObject* object, bool enabled)
{
    return {EngineUpdate::Verb::AlterObject, (EngineUpdate::Noun)object, (EngineUpdate::Noun)enabled};
}

inline const EngineUpdate RelinkPorts(ModuleSink* sink, ModuleSource* source = nullptr)
{
    return {EngineUpdate::Verb::RelinkPorts, (EngineUpdate::Noun)sink, (EngineUpdate::Noun)source};
}


class Engine{
    Engine();
    Engine(const Engine&) {}

public:
    static Engine* singletonInstance();

    bool update(const EngineUpdate* transaction, unsigned long size);

    inline bool update(const std::vector<EngineUpdate> &transaction)
        {  return update(transaction.data(), transaction.size()); }

};

}//namespace r64fx

#endif//R64FX_ENGINE_HPP