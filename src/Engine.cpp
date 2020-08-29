#include "Engine.hpp"
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "CircularBuffer.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

struct ModuleImpl{
};

struct ModuleImpl_SoundDriverSource : public ModuleImpl{
    SoundDriverAudioInput* sd_audio_input = nullptr;
};

struct ModuleImpl_SoundDriverSink : public ModuleImpl{
    SoundDriverAudioOutput* sd_audio_output = nullptr;
};


struct EngineDspStep{
    std::vector<EngineDspStep*> prev_steps;
    std::vector<EngineObject*> processed_objects;
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


class EngineImpl{
    Module_SoundDriver* m_module_sound_driver = nullptr;

//     void startSoundDriver(EngineData* ed)
//     {
//         cout << "StartSoundDriver\n";
//         sound_driver = SoundDriver::newInstance(SoundDriver::Type::Jack, "r64fx");
//         if(m_module_sound_driver)
//         {
//             m_module_sound_driver->enable();
//         }
//     }
// 
//     void stopSoundDriver(EngineData* ed)
//     {
//         cout << "StopSoundDriver\n";
//         if(msound_driver)
//         {
//             m_module_sound_driver->disable();
//             SoundDriver::deleteInstance(m_module_sound_driver);
//         }
//     }

public:
    bool update(const EngineUpdate* transaction, unsigned long size);

private:
    void verbAddObject(Module* parent, EngineObject* child);
    void engageObject(EngineObject* child);
    void engageModule(Module* module);
    void engagePort(ModulePort* port);

    void verbReplaceRoot(Module* module);
    void verbRemoveObject(Module* parent, EngineObject* child);
    void verbReplaceObject(Module* parent, EngineObject* old_object, EngineObject* new_object);
    void verbAlterObject(EngineObject* object, bool enabled);
    void verbRelinkPorts(ModuleSink* sink, ModuleSource* source);

    void rebuild();
    void rebuildDspStep(EngineDspStep* dsp_step);
    void rebuildDspStepSources(Module* module, std::vector<EngineDspStep*> &prev_steps);

} g_engine_impl;


Engine* Engine::singletonInstance()
{
    return (Engine*)&g_engine_impl;
}


bool Engine::update(const EngineUpdate* transaction, unsigned long size)
{
    auto impl = (EngineImpl*)this;
    return impl->update(transaction, size);
}


bool EngineImpl::update(const EngineUpdate* transaction, unsigned long size)
{
    for(auto eu = transaction; eu != transaction+size; eu++)
    {
        switch(eu->verb)
        {
            case EngineUpdate::Verb::ReplaceRoot:
            {
                verbReplaceRoot((Module*)(eu->noun[0]));
                break;
            }

            case EngineUpdate::Verb::AddObject:
            {
                verbAddObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::RemoveObject:
            {
                verbRemoveObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::ReplaceObject:
            {
                verbReplaceObject((Module*)(eu->noun[0]), (EngineObject*)(eu->noun[1]), (EngineObject*)(eu->noun[2]));
                break;
            }

            case EngineUpdate::Verb::AlterObject:
            {
                verbAlterObject((EngineObject*)(eu->noun[0]), (bool)(eu->noun[1]));
                break;
            }

            case EngineUpdate::Verb::RelinkPorts:
            {
                verbRelinkPorts((ModuleSink*)(eu->noun[0]), (ModuleSource*)(eu->noun[1]));
                break;
            }

            default:
            {
                R64FX_DEBUG_ABORT("Bad EngineUpdate Verb!\n");
                break;
            }
        }
    }

    rebuild();

    return true;
}


void EngineImpl::verbReplaceRoot(Module* module)
{
    R64FX_DEBUG_ASSERT(module);
    engageObject(module);
}


void EngineImpl::verbAddObject(Module* parent, EngineObject* child)
{
    R64FX_DEBUG_ASSERT(child);
    R64FX_DEBUG_ASSERT(child->impl == nullptr);

    parent->addObject(child);
    engageObject(child);
}


void EngineImpl::engageObject(EngineObject* object)
{
    if(object->isModule())
        engageModule((Module*)object);
    else if(object->isPort())
        engagePort((ModulePort*)object);
    else
        R64FX_DEBUG_ASSERT("Bad EngineObject type!\n");
}


void EngineImpl::engageModule(Module* module)
{
    switch(module->type)
    {
        case EngineObjectType<Module_SoundDriver>():
        {
            R64FX_DEBUG_ASSERT(m_module_sound_driver == nullptr);
            R64FX_DEBUG_ASSERT(!module->name.empty());

            m_module_sound_driver = (Module_SoundDriver*)module;
            m_module_sound_driver->impl =
                SoundDriver::newInstance(SoundDriver::Type::Jack, module->name.c_str());
            break;
        }

        default:
        {
            break;
        }
    }

    for(auto object : module->objects)
        engageObject(object);
}


void EngineImpl::engagePort(ModulePort* port)
{
    R64FX_DEBUG_ASSERT(port->parent);
    R64FX_DEBUG_ASSERT(port->parent->isModule());

    auto module = (Module*)port->parent;

    switch(module->type)
    {
        case EngineObjectType<Module_SoundDriver>():
        {
            auto msd = (Module_SoundDriver*) module;
            auto sd = (SoundDriver*) msd->impl;

            switch(port->type)
            {
                case EngineObjectType<ModuleSink>():
                {
                    if(port->nchannels == 0)
                    {
                        port->impl = sd->newMidiOutput(port->name);
                    }
                    else
                    {
                        auto sdports = new SoundDriverAudioOutput*[port->nchannels];
                        for(unsigned long i=0; i<port->nchannels; i++)
                            sdports[i] = sd->newAudioOutput(port->name + std::to_string(i+1));
                        port->impl = sdports;
                    }
                    break;
                }

                case EngineObjectType<ModuleSource>():
                {
                    if(port->nchannels == 0)
                    {
                        port->impl = sd->newMidiInput(port->name);
                    }
                    else
                    {
                        auto sdports = new SoundDriverAudioInput*[port->nchannels];
                        for(unsigned long i=0; i<port->nchannels; i++)
                            sdports[i] = sd->newAudioInput(port->name + std::to_string(i+1));
                        port->impl = sdports;
                    }
                    break;
                }

                default:
                {
                    R64FX_DEBUG_ABORT("engagePort: Bad port type!\n");
                    break;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
}


void EngineImpl::verbRemoveObject(Module* parent, EngineObject* child)
{
    R64FX_DEBUG_ASSERT(parent);
    R64FX_DEBUG_ASSERT(child);

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

    R64FX_DEBUG_ABORT("verbRemoveObject: Parent has no such child!\n");
}


void EngineImpl::verbReplaceObject(Module* parent, EngineObject* old_object, EngineObject* new_object)
{
    R64FX_DEBUG_ASSERT(old_object);
    R64FX_DEBUG_ASSERT(new_object);

    for(unsigned int i=0; i<parent->objects.size(); i++)
    {
        if(parent->objects[i] == old_object)
        {
            parent->objects[i] = new_object;
            return;
        }
    }

    R64FX_DEBUG_ABORT("verbReplaceObject: Parent has no such child!\n");
}


void EngineImpl::verbAlterObject(EngineObject* object, bool enabled)
{
    R64FX_DEBUG_ASSERT(object);

    switch(object->type)
    {
        case EngineObjectType<Module_SoundDriver>():
        {
            auto msd = (Module_SoundDriver*) object;
            R64FX_DEBUG_ASSERT(msd->impl);

            auto sd = (SoundDriver*) msd->impl;
            if(enabled)
            {
                cout << "AAA\n";
                sd->enable();
            }
            else
            {
                cout << "BBB\n";
                sd->disable();
            }
            break;
        }

        default:
        {
            break;
        }
    }
}


void EngineImpl::verbRelinkPorts(ModuleSink* sink, ModuleSource* source)
{
    R64FX_DEBUG_ASSERT(sink);
}


void EngineImpl::rebuild()
{
    if(!m_module_sound_driver)
        return;

    auto dsp_step = new EngineDspStep;
    dsp_step->processed_objects.push_back(m_module_sound_driver);
    rebuildDspStep(dsp_step);
}


void EngineImpl::rebuildDspStep(EngineDspStep* dsp_step)
{
    R64FX_DEBUG_ASSERT(dsp_step);
    R64FX_DEBUG_ASSERT(dsp_step->prev_steps.empty());

    if(dsp_step->processed_objects.empty())
        return;

    for(auto object : dsp_step->processed_objects)
    {
        R64FX_DEBUG_ASSERT(object->isModule());
        rebuildDspStepSources((Module*)object, dsp_step->prev_steps);
    }

    for(auto &step : dsp_step->prev_steps)
    {
        rebuildDspStep(step);
    }
}


void EngineImpl::rebuildDspStepSources(Module* module, std::vector<EngineDspStep*> &prev_steps)
{
    for(auto object : module->objects)
    {
        if(object->type != EngineObjectType<ModuleSink>())
            continue;

        auto sink = (ModuleSink*) object;

        if(!sink->connected_source)
            continue;

        auto source = (ModuleSource*) sink->connected_source;
        R64FX_DEBUG_ASSERT(source->parent);
        R64FX_DEBUG_ASSERT(source->parent->isModule());

        auto connected_module = (Module*) source->parent;

        EngineDspStep* dsp_step = nullptr;

        for(auto step : prev_steps)
        {
            R64FX_DEBUG_ASSERT(!step->processed_objects.empty());
            if(step->processed_objects[0]->type == connected_module->type)
            {
                dsp_step = step;
            }
        }

        if(dsp_step == nullptr)
        {
            dsp_step = new EngineDspStep;
            prev_steps.push_back(dsp_step);
        }

        dsp_step->processed_objects.push_back(connected_module);
    }
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