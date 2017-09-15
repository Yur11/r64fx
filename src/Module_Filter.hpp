#ifndef R64FX_MODULE_FILTER_HPP
#define R64FX_MODULE_FILTER_HPP

#include "Module.hpp"
#include "FilterClass.hpp"


namespace r64fx{

class Module_Filter : public Module{
    void* m = nullptr;

public:
    Module_Filter();

    virtual ~Module_Filter();

    void setFilterClass(FilterClass* fc);

    virtual bool engage(Module::Callback* done = nullptr, void* done_arg = nullptr, ModuleThreadHandle* threads = nullptr, int nthreads = 0);

    virtual void disengage(Module::Callback* done = nullptr, void* done_arg = nullptr);

    virtual bool isEngaged();

    virtual bool engagementPending();
};

}//namespace r64fx

#endif//R64FX_MODULE_FILTER_HPP
