#ifndef R64FX_MODULE_SOUND_DRIVER_HPP
#define R64FX_MODULE_SOUND_DRIVER_HPP

#include "Module.hpp"

namespace r64fx{

typedef void (Callback_AddAudioInput)(ModuleSource* source, void* arg1, void* arg2);
typedef void (Callback_AddAudioOutput)(ModuleSink* sink, void* arg1, void* arg2);
typedef void (Callback_RemovePort)(void* arg1, void* arg2);

class Module_SoundDriver : public Module{
    void* m = nullptr;

public:
    Module_SoundDriver();

    virtual ~Module_SoundDriver();

    virtual bool engage(
        Module::Callback* done = nullptr, void* done_arg = nullptr, ModuleThreadHandle* threads = nullptr, int nthreads = 0
    ) override final;

    virtual void disengage(Module::Callback* done = nullptr, void* done_arg = nullptr) override final;

    virtual bool isEngaged() override final;

    virtual bool engagementPending() override final;

    void addAudioInput(const char* name, Callback_AddAudioInput* response, void* arg1, void* arg2);

    void addAudioOutput(const char* name, Callback_AddAudioOutput* response, void* arg1, void* arg2);

    void removePort(ModulePort* port, Callback_RemovePort* response, void* arg1, void* arg2);
};

}//namespace r64fx

#endif//R64FX_MODULE_SOUND_DRIVER_HPP
