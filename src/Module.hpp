#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

namespace r64fx{

class Module{
    friend class ModulePrivate;
    void* m = nullptr;

protected:
    Module();

    virtual ~Module();
};

typedef void (*ModuleCallback)(Module* module, void* arg);

}//namespace r64fx

#endif//R64FX_MODULE_HPP
