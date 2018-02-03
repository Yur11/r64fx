#ifndef R64FX_MODULE_SINE_GENERATOR_HPP
#define R64FX_MODULE_SINE_GENERATOR_HPP

#include "Module.hpp"

namespace r64fx{

class Module_SineGenerator : public Module{
    void* m = nullptr;

public:
    Module_SineGenerator();

    virtual ~Module_SineGenerator();

    virtual bool engage(Module::Callback* done = nullptr, void* done_arg = nullptr, ModuleThreadHandle* threads = nullptr, int nthreads = 0);

    virtual void disengage(Module::Callback* done = nullptr, void* done_arg = nullptr);

    virtual bool isEngaged();

    virtual bool engagementPending();

    ModuleSource* source() const;
};

}//namespace r64fx

#endif//R64FX_MODULE_SINE_GENERATOR_HPP
