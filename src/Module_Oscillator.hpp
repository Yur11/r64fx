#ifndef R64FX_MODULE_OSCILLATOR_HPP
#define R64FX_MODULE_OSCILLATOR_HPP

#include "Module.hpp"

namespace r64fx{

class Module_Oscillator : public Module{
    void* m = nullptr;

public:
    Module_Oscillator();

    virtual ~Module_Oscillator();

    virtual bool engage(ModuleCallback done = nullptr, void* done_arg = nullptr);

    virtual void disengage(ModuleCallback done = nullptr, void* done_arg = nullptr);

    virtual bool isEngaged();

    virtual bool engagementPending();
};

}//namespace r64fx

#endif//R64FX_MODULE_OSCILLATOR_HPP
