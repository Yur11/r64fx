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

}//namespace r64fx

#endif//R64FX_MODULE_HPP
