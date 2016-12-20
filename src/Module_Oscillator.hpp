#ifndef R64FX_MODULE_OSCILLATOR_HPP
#define R64FX_MODULE_OSCILLATOR_HPP

#include "Module.hpp"

namespace r64fx{

class Module_Oscillator : public Module{
public:
    Module_Oscillator();

    virtual ~Module_Oscillator();

    void changeFrequency(float freq, ModuleCallback done, void* done_arg);
};

}//namespace r64fx

#endif//R64FX_MODULE_OSCILLATOR_HPP
