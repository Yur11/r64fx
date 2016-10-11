#ifndef R64FX_PROGRAM_ACTION_HPP
#define R64FX_PROGRAM_ACTION_HPP

#include "Action.hpp"

namespace r64fx{
    
struct ProgramActions{
    
    Action* new_session_act      = nullptr;
    Action* open_session_act     = nullptr;
    Action* save_session_act     = nullptr;
    Action* save_session_as_act  = nullptr;
    Action* quit_act             = nullptr;
    
    Action* new_project_act      = nullptr;
    Action* open_project_act     = nullptr;
    Action* save_project_act     = nullptr;
    Action* save_project_as_act  = nullptr;
    Action* close_project_act    = nullptr;
    
    Action* cut_act              = nullptr;
    Action* copy_act             = nullptr;
    Action* paste_act            = nullptr;
    Action* undo_act             = nullptr;
    Action* redo_act             = nullptr;
    
    Action* no_view_act          = nullptr;
    
    Action* no_help_act          = nullptr;
};

#ifndef R64FX_PROGRAM_ACTIONS_IMPL
extern ProgramActions* g_acts;
#endif//R64FX_PROGRAM_ACTIONS_IMPL
    
}//namespace r64fx

#endif//R64FX_PROGRAM_ACTION_HPP
