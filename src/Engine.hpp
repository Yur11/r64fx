#ifndef R64FX_ENGINE_HPP
#define R64FX_ENGINE_HPP

#include <vector>
#include <string>

#include "Debug.hpp"

namespace r64fx{

/*
 * EngineObject base class
 */
class EngineObject{
    void* m = nullptr; // Implementation placeholder

public:
    EngineObject* parent  = nullptr;

    std::string name    = "";

    void remove();
    void replaceWith(EngineObject*);

    /*
     *  Type Id system for engine objects
     */
    typedef unsigned short TypeId;

#   define EO_TYPE(Name, TypeBits)                                      \
    class Name;                                                         \
    constexpr static TypeId TypeIdOf(Name* obj) { return (TypeBits); }  \
    inline bool is##Name() const { return is<Name>(); }                 \
    inline Name* to##Name() const { return to<Name>(); }

    constexpr static TypeId PortBit = 0x8000;
    constexpr static TypeId NodeBit = 0x4000;

    EO_TYPE(Port,           0 | PortBit)
    EO_TYPE(Source,         1 | PortBit)
    EO_TYPE(Sink,           2 | PortBit)

    EO_TYPE(Node,           0 | NodeBit)
    EO_TYPE(SoundDriver,    1 | NodeBit)
    EO_TYPE(Test,           2 | NodeBit)

#   undef EO_TYPE

    template<typename T> constexpr static TypeId Type(T* obj = nullptr)
        { return TypeIdOf((T*)obj); }

    template<typename T> inline bool is() const
        { return type() == Type<T>(); }

    template<typename T> inline T* to() const
        { R64FX_DEBUG_ASSERT(is<T>()); return (T*)this; }

    inline TypeId type() const { return m_type; }

protected:
    EngineObject(TypeId type) : m_type(type)
    {
        R64FX_DEBUG_ASSERT(m_type != 0)
    }

private:
    const TypeId m_type = 0;
};

typedef EngineObject EO; // Convenience alias

/* Intermediate EO class protected constructor */
#define EO_BASE(This, Parent) This(EO::TypeId type) : Parent(type)

/* Leaf EO class public constructor */
#define EO_CTOR(This, Parent) This() : Parent(EO::Type<This>())


/* Base port class */
struct EO::Port : public EO{
    unsigned int chan_count = 0;

protected:
    EO_BASE(Port, EO) {}
};


/* Source port that can be connected to sinks */
struct EO::Source : public EO::Port{
    EO_CTOR(Source, Port) {}
};


/* Sink port that can be connected to source ports */
struct EO::Sink : public EO::Port{
    EO::Port* connected_port = nullptr;

    EO_CTOR(Sink, Port) {}
};


/* Node EO object that cointains other EOs */
struct EO::Node : public EO{
    std::vector<EO*> objects;

    void add(EO* eo);

    EO_BASE(Node, EO) {}
};

inline void EO::Node::add(EO* eo)
{
    R64FX_DEBUG_ASSERT(eo != nullptr);
    R64FX_DEBUG_ASSERT(eo->parent == nullptr);

    eo->parent = this;
    this->objects.push_back(eo);
}

inline void EO::remove()
{
    R64FX_DEBUG_ASSERT(parent != nullptr);
    R64FX_DEBUG_ASSERT(parent->is<EO::Node>());

    auto p = parent->to<EO::Node>();
    auto &o = p->objects;

    unsigned int i=0;
    while(i < o.size() && o[i] != this)
        i++;
    if(i == o.size() - 1)
        o.pop_back();
    else
        while(i < o.size() - 1)
            o[i] = o[i + 1];
    this->parent = nullptr;
}

inline void EO::replaceWith(EO* eo)
{
    R64FX_DEBUG_ASSERT(eo != nullptr);
    R64FX_DEBUG_ASSERT(eo->parent == nullptr);
    R64FX_DEBUG_ASSERT(this->parent != nullptr);

    auto p = parent->to<EO::Node>();
    auto &o = p->objects;

    for(unsigned int i=0; i<o.size(); i++)
        if(o[i] == this)
            { o[i] = eo; break; }
    eo->parent = this->parent;
}


struct EO::SoundDriver : public EO::Node{
    EO_CTOR(SoundDriver, Node) {}
};


struct EO::Test : public EO::Node{
    EO_CTOR(Test, Node) {}
};

#undef EO_BASE
#undef EO_CTOR


struct EngineUpdate{
    enum class Verb{
        None, Init, Add, Remove, Replace, Alter, Link
    };

    typedef unsigned long Noun;

    Verb verb = Verb::None;
    Noun noun[3] = {0, 0, 0};

    EngineUpdate(Verb verb, Noun first = 0, Noun second = 0, Noun third = 0)
        : verb(verb), noun{first, second, third} {}
};

typedef EngineUpdate EU; // Convenience alias

inline const EU Init(EO::Node* node)
{
    return {EU::Verb::Init, EU::Noun(node)};
}

inline const EU Add(EO::Node* parent, EO* child)
{
    return {EU::Verb::Add, EU::Noun(parent), EU::Noun(child)};
}

inline const EU Remove(EO* child)
{
    return {EU::Verb::Remove, EU::Noun(child)};
}

inline const EU Replace(EO* cur_object, EO* new_object)
{
    return {EU::Verb::Replace, (EU::Noun)cur_object, (EU::Noun)new_object};
}

inline const EU Alter(EO* object, bool enabled)
{
    return {EU::Verb::Alter, EU::Noun(object), EU::Noun(enabled)};
}

inline const EU Link(EO::Sink* sink, EO::Source* source = nullptr)
{
    return {EU::Verb::Link, EU::Noun(sink), EU::Noun(source)};
}


class Engine{
    Engine();
    Engine(const Engine&) {}
    ~Engine();

public:
    static Engine* newInstance();

    static void deleteInstance(Engine* engine);

    bool update(const EU* transaction, unsigned long size);

    inline bool update(const std::vector<EU> &transaction)
        {  return update(transaction.data(), transaction.size()); }

};

}//namespace r64fx

#endif//R64FX_ENGINE_HPP