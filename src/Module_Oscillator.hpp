#ifndef R64FX_MODULE_OSCILLATOR_HPP
#define R64FX_MODULE_OSCILLATOR_HPP

#include "Module.hpp"

namespace r64fx{

class Module_Oscillator : public Module{
    void* m = nullptr;

public:
    Module_Oscillator();

    virtual ~Module_Oscillator();

    virtual void engage();

    virtual void disengage();

    virtual bool isEngaged();
};

}//namespace r64fx

#endif//R64FX_MODULE_OSCILLATOR_HPP
