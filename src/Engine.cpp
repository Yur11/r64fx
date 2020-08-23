#include "Engine.hpp"
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "CircularBuffer.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

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

class EngineImpl{
    friend class Engine;

    SoundDriver* m_sound_driver = nullptr;
    EngineObject* m_root_object = nullptr;

//     void startSoundDriver(EngineData* ed)
//     {
//         cout << "StartSoundDriver\n";
//         sound_driver = SoundDriver::newInstance(SoundDriver::Type::Jack, "r64fx");
//         if(m_sound_driver)
//         {
//             m_sound_driver->enable();
//         }
//     }
// 
//     void stopSoundDriver(EngineData* ed)
//     {
//         cout << "StopSoundDriver\n";
//         if(msound_driver)
//         {
//             m_sound_driver->disable();
//             SoundDriver::deleteInstance(m_sound_driver);
//         }
//     }

    void verbAddObject(Module* parent, EngineObject* child);
    void verbRemoveObject(Module* parent, EngineObject* child);
    void verbReplaceObject(Module* parent, EngineObject* old_object, EngineObject* new_object);
    void verbAlterObject(EngineObject* object, bool enabled);
    void verbRelinkPorts(ModuleSink* sink, ModulePort* port);

    void rebuild();

} g_engine_impl;


Engine* Engine::singletonInstance()
{
    return (Engine*)&g_engine_impl;
}


bool Engine::update(
    EngineUpdate* transaction, unsigned long size,
    void (*done)(void* arg, EngineUpdate* transaction, unsigned long size), void* arg
)
{
    auto impl = (EngineImpl*)this;

    for(auto eu = transaction; eu != transaction+size; eu++)
    {
        switch(eu->verb)
        {
            case EngineUpdate::Verb::AddObject:
            {
                impl->verbAddObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::RemoveObject:
            {
                impl->verbRemoveObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::ReplaceObject:
            {
                impl->verbReplaceObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]), (EngineObject*)(eu->noun[2]));
                break;
            }

            case EngineUpdate::Verb::AlterObject:
            {
                impl->verbAlterObject((EngineObject*)(eu->noun[0]), (bool)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::RelinkPorts:
            {
                impl->verbRelinkPorts((ModuleSink*)(eu->noun[0]), (ModulePort*)(eu->noun[1]));
                break;
            }

            default:
            {
                R64FX_DEBUG_ABORT("Bad EngineUpdate Verb!");
                break;
            }
        }
    }

    impl->rebuild();

    return true;
}


void EngineImpl::verbAddObject(Module* parent, EngineObject* child)
{
    R64FX_DEBUG_ASSERT(child != nullptr);

    if(parent)
        m_root_object = child;
    else
        parent->objects.push_back(child);
}


void EngineImpl::verbRemoveObject(Module* parent, EngineObject* child)
{
    R64FX_DEBUG_ASSERT(parent != nullptr);
    R64FX_DEBUG_ASSERT(child != nullptr);

    for(unsigned int i=0; i<parent->objects.size(); i++)
    {
        if(parent->objects[i] == child)
        {
            if(i < parent->objects.size() - 1)
                parent->objects[i] = parent->objects[parent->objects.size() - 1];
            parent->objects.pop_back();
            return;
        }
    }

    R64FX_DEBUG_ABORT("RemoveObject: Parent has no such child!");
}


void EngineImpl::verbReplaceObject(Module* parent, EngineObject* old_object, EngineObject* new_object)
{
    R64FX_DEBUG_ASSERT(old_object != nullptr);
    R64FX_DEBUG_ASSERT(new_object != nullptr);

    for(unsigned int i=0; i<parent->objects.size(); i++)
    {
        if(parent->objects[i] == old_object)
        {
            parent->objects[i] = new_object;
            return;
        }
    }

    R64FX_DEBUG_ABORT("ReplaceObject: Parent has no such child!");
}


void EngineImpl::verbAlterObject(EngineObject* object, bool enabled)
{
    R64FX_DEBUG_ASSERT(object != nullptr);
}


void EngineImpl::verbRelinkPorts(ModuleSink* sink, ModulePort* port)
{
    R64FX_DEBUG_ASSERT(sink != nullptr);
}


void EngineImpl::rebuild()
{
    
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