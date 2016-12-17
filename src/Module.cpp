#include "Module.hpp"
#include "ModulePrivate.hpp"
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"

namespace r64fx{

namespace{

struct ModuleGlobal{
    LinkedList<ThreadObjectIface> thread_roots;
} g_module;

}//namespace


Module::Module()
{
    
}


Module::~Module()
{
    
}

}//namespace r64fx
