#ifndef R64FX_MODULE_SOUND_DRIVER_HPP
#define R64FX_MODULE_SOUND_DRIVER_HPP

#include "Module.hpp"

namespace r64fx{

class ModuleSoundDriverInputSource : public ModuleSource{};
class ModuleSoundDriverOutputSink : public ModuleSink{};

typedef void (Callback_AddAudioInput)     (ModuleSoundDriverInputSource* source, void* arg1, void* arg2);
typedef void (Callback_AddAudioOutput)    (ModuleSoundDriverOutputSink* sink, void* arg1, void* arg2);
typedef void (Callback_RemoveAudioInput)  (void* arg1, void* arg2);
typedef void (Callback_RemoveAudioOutput) (void* arg1, void* arg2);

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

    void addAudioInput(const char* name, unsigned int chan_count, Callback_AddAudioInput* response, void* arg1, void* arg2);

    void addAudioOutput(const char* name, unsigned int chan_count, Callback_AddAudioOutput* response, void* arg1, void* arg2);

    void removePort(ModuleSoundDriverInputSource* source, Callback_RemoveAudioInput* response, void* arg1, void* arg2);

    void removePort(ModuleSoundDriverOutputSink* sink,  Callback_RemoveAudioOutput* response, void* arg1, void* arg2);
};

}//namespace r64fx

#endif//R64FX_MODULE_SOUND_DRIVER_HPP
