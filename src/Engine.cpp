#include <iostream>

#include "Engine.hpp"
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "CircularBuffer.hpp"
#include "JitProc.hpp"


using std::cout;

namespace r64fx{

struct EngineDspStep{
    std::vector<EO::Node*> nodes;
    std::vector<EngineDspStep*> source_steps, sink_steps;

    EngineDspStep() {}
    EngineDspStep(EO::Node* node) { addNode(node); }

    void addNode(EO::Node* node);

    void addSourceStep(EngineDspStep* dsp);

    inline EO::TypeId type() const
    {
        if(nodes.empty())
            return 0;

        return nodes[0]->type();
    }
};

typedef EngineDspStep DSP;


struct NodeImpl{
    struct P{
        DSP* dsp = nullptr;
        int slot = 0;
    } cur_impl, new_impl;

    inline static NodeImpl* &of(EO::Node* node)
        { return *((NodeImpl**)node); }

    inline bool isPending()
        { return new_impl.dsp != nullptr; }

    inline void commit()
        { cur_impl = new_impl; new_impl = P(); }

    inline void revert()
        { new_impl = P(); }

    inline void clear()
        { cur_impl = new_impl = P(); }
};


struct EngineWorker{
    struct Message{
        enum class Verb{
            None, Stop, Update
        };

        Verb verb = Verb::None;
        void *first = nullptr, *second = nullptr, *third = nullptr;
    };

    struct CommBuffers{
        CircularBuffer<Message> iface2impl, impl2iface;
        CommBuffers() : iface2impl(8), impl2iface(8) {}
    };

    class Iface : private CommBuffers{
        Iface(){}
        Iface(const Iface&) {}

    public:
        inline bool send(const Message &msg)
            { return iface2impl.write(&msg, 1); }

        inline bool recv(Message &msg)
            { return impl2iface.read(&msg, 1); }
    };

    class Impl : private CommBuffers{
        Impl() {}
        Impl(const Impl&) {}

    public:
        inline bool send(const Message &msg)
            { return impl2iface.write(&msg, 1); }

        inline bool recv(Message &msg)
            { return iface2impl.read(&msg, 1); }
    };

    static Iface* newWorker()
    {
        return nullptr;
    }

    static void thread(EngineWorker::Impl* worker);
};

typedef EngineWorker EW;


class EngineImpl{
    EO::SoundDriver* m_eo_soubd_driver = nullptr;
    EO* m_eo_root = nullptr;

public:
    bool update(const EU* transaction, unsigned long size);

private:
    /* Engine update verbs */
    void verbInit(EO::Node*); void verbAdd(EO::Node*, EO*); void verbRemove(EO*);
    void verbReplace(EO*, EO*); void verbAlter(EO*, bool); void verbLink(EO::Sink*, EO::Source*);

    void updateDspGraph();

    /* Find sound driver object in engine object tree */
    EO::SoundDriver* findSoundDriver(EO* eo);

    /* Generate new dsp step graph */
    void genDspSteps(EngineDspStep* step);
};

typedef EngineImpl EI;


Engine* Engine::newInstance()
    { return (Engine*) new EngineImpl; }


void Engine::deleteInstance(Engine* engine)
    { delete (EngineImpl*) engine; }


bool Engine::update(const EU* transaction, unsigned long size)
{
    auto impl = (EngineImpl*)this;
    return impl->update(transaction, size);
}


bool EngineImpl::update(const EU* transaction, unsigned long size)
{
    for(auto eu = transaction; eu != transaction+size; eu++)
    {
        auto n = eu->noun;

        switch(eu->verb){
#       define EU_VERB(V) case EU::Verb::V: verb##V
        EU_VERB(Init)     ( (EO::Node*) (n[0])                        ); break;
        EU_VERB(Add)      ( (EO::Node*) (n[0]),  (EO*)         (n[1]) ); break;
        EU_VERB(Remove)   ( (EO*)       (n[0])                        ); break;
        EU_VERB(Replace)  ( (EO*)       (n[0]),  (EO*)         (n[1]) ); break;
        EU_VERB(Alter)    ( (EO*)       (n[0]),  (bool)        (n[1]) ); break;
        EU_VERB(Link)     ( (EO::Sink*) (n[0]),  (EO::Source*) (n[1]) ); break;
#       undef EU_VERB

        default:
            R64FX_DEBUG_ABORT("Bad EU Verb!\n");
            break;
        }
    }

    updateDspGraph();

    return true;
}


void EI::verbInit(EO::Node* root)
{
    R64FX_DEBUG_ASSERT(root);
    m_eo_root = root;
}


void EI::verbAdd(EO::Node* parent, EngineObject* child)
{
    R64FX_DEBUG_ASSERT(parent != nullptr)
    R64FX_DEBUG_ASSERT(child != nullptr);
    parent->add(child);
}


void EI::verbRemove(EngineObject* child)
{
    R64FX_DEBUG_ASSERT(child);
    child->remove();
}


void EI::verbReplace(EO* cur_object, EO* new_object)
{
    R64FX_DEBUG_ASSERT(cur_object);
    R64FX_DEBUG_ASSERT(new_object);
    cur_object->replaceWith(new_object);
}


void EI::verbAlter(EO* object, bool enabled)
{
    R64FX_DEBUG_ASSERT(object);
}


void EI::verbLink(EO::Sink* sink, EO::Source* source)
{
    R64FX_DEBUG_ASSERT(sink);
    sink->connected_port = source;
}


EO::SoundDriver* EI::findSoundDriver(EO* eo)
{
    if(eo->isSoundDriver())
        return (EO::SoundDriver*) eo;

    if(!eo->isNode())
        return nullptr;

    auto &o = eo->toNode()->objects;
    for(unsigned int i=0; i<o.size(); i++)
    {
        auto sd = findSoundDriver(o[i]);
        if(sd) return sd;
    }

    return nullptr;
}


void EI::updateDspGraph()
{
    auto sd = findSoundDriver(m_eo_root);
    R64FX_DEBUG_ASSERT(sd != nullptr);

    auto dsp = new DSP(sd);
    genDspSteps(dsp);
}


void EI::genDspSteps(DSP* dsp)
{
    R64FX_DEBUG_ASSERT(dsp != nullptr);

    std::vector<DSP*> new_dsp_steps;

    for(auto node : dsp->nodes)
    {
        for(auto obj : node->objects)
        {
            if(!obj->isSink())
                continue;

            auto sink = obj->toSink();
            if(sink->connected_port == nullptr)
                continue; // No connection on this port.

            R64FX_DEBUG_ASSERT(sink->connected_port->isSource()); //Fix me!

            R64FX_DEBUG_ASSERT(sink->connected_port->parent->isNode());
            auto source_node = sink->connected_port->parent->toNode();

            if(NodeImpl::of(source_node)->isPending())
            {
                // Source node is already attached to a DSP step.
                // Just make sure there is a link between dsp and source_dsp.

                dsp->addSourceStep(NodeImpl::of(source_node)->new_impl.dsp);
            }
            else
            {
                // Find exising source DSP step or create a new one.
                DSP* source_dsp = nullptr;

                for(auto sdsp : dsp->source_steps)
                {
                    if(sdsp->type() == source_node->type())
                    {
                        source_dsp = sdsp;
                        break;
                    }
                }

                if(source_dsp == nullptr)
                {
                    source_dsp = new DSP;
                    dsp->addSourceStep(source_dsp);
                    new_dsp_steps.push_back(source_dsp);
                }

                source_dsp->addNode(source_node);
            }
        }
    }

    // Recursively process newly created DSP steps.
    for(auto ndsp : new_dsp_steps)
        genDspSteps(ndsp);
}


void DSP::addNode(EO::Node* node)
{
    R64FX_DEBUG_ASSERT(node);

    if(!nodes.empty())
    {
        R64FX_DEBUG_ASSERT(nodes[0]->type() == node->type());
    }

    if(NodeImpl::of(node) == nullptr)
    {
        NodeImpl::of(node) = new NodeImpl;
    }

    if(NodeImpl::of(node)->new_impl.dsp == nullptr)
    {
        nodes.push_back(node);
        NodeImpl::of(node)->new_impl.dsp = this;
    }
}


void DSP::addSourceStep(EngineDspStep* dsp)
{
    for(auto sdsp : this->source_steps)
    {
        if (dsp == sdsp)
            return; // Already added!
    }

    this->source_steps.push_back(dsp);
    dsp->sink_steps.push_back(this);
}


void EngineWorker::thread(EngineWorker::Impl* worker)
{
    bool running = true;

    while(running)
    {
        sleep_nanoseconds(500000);
    }
}

}//namespace r64fx