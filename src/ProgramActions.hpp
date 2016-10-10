#ifndef R64FX_PROGRAM_ACTION_HPP
#define R64FX_PROGRAM_ACTION_HPP

#include "Action.hpp"

namespace r64fx{
    
struct ProgramActions{
    Action* quit_act = nullptr;
};

#ifndef R64FX_PROGRAM_ACTIONS_IMPL
extern ProgramActions* g_acts;
#endif//R64FX_PROGRAM_ACTIONS_IMPL
    
}//namespace r64fx

#endif//R64FX_PROGRAM_ACTION_HPP
