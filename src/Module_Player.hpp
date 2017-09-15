#ifndef R64FX_MODULE_PLAYER_HPP
#define R64FX_MODULE_PLAYER_HPP

#include "Module.hpp"

namespace r64fx{

class Module_Player : public Module{
    void* m = nullptr;

public:
    Module_Player();

    virtual ~Module_Player();

    void change(float* buff, int nchans, int nframes);

    virtual bool engage(Module::Callback* done = nullptr, void* done_arg = nullptr, ModuleThreadHandle* threads = nullptr, int nthreads = 0) override final;

    virtual void disengage(Module::Callback* done = nullptr, void* done_arg = nullptr) override final;

    virtual bool isEngaged() override;

    virtual bool engagementPending() override;
};

}//namespace r64fx

#endif//R64FX_MODULE_PLAYER_HPP
