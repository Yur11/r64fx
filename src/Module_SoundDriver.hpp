#ifndef R64FX_MODULE_SOUND_DRIVER_HPP
#define R64FX_MODULE_SOUND_DRIVER_HPP

#include "Module.hpp"

namespace r64fx{

typedef void (Response_AddAudioInput)(ModuleSource* source, void* arg1, void* arg2);
typedef void (Response_AddAudioOutput)(ModuleSink* source, void* arg1, void* arg2);

class Module_SoundDriver : public Module{
    void* m = nullptr;

public:
    Module_SoundDriver();

    virtual ~Module_SoundDriver();

    virtual bool engage(ModuleCallback done = nullptr, void* done_arg = nullptr) override;

    virtual void disengage(ModuleCallback done = nullptr, void* done_arg = nullptr) override;

    virtual bool isEngaged() override;

    virtual bool engagementPending() override;

    void addAudioInput(const char* name, Response_AddAudioInput* response, void* arg1, void* arg2);

    void addAudioOutput(const char* name, Response_AddAudioOutput* response, void* arg1, void* arg2);
};

}//namespace r64fx

#endif//R64FX_MODULE_SOUND_DRIVER_HPP
