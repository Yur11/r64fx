#ifndef R64FX_MODULE_PLAYER_HPP
#define R64FX_MODULE_PLAYER_HPP

#include "Module.hpp"

namespace r64fx{

class Module_Player : public Module{
    void* m = nullptr;

public:
    Module_Player();

    virtual ~Module_Player();

    virtual bool engage(ModuleCallback done = nullptr, void* done_arg = nullptr) override;

    virtual void disengage(ModuleCallback done = nullptr, void* done_arg = nullptr) override;

    virtual bool isEngaged() override;

    virtual bool engagementPending() override;
};

}//namespace r64fx

#endif//R64FX_MODULE_PLAYER_HPP
