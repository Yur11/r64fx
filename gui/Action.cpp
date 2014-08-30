#include "Action.hpp"

namespace r64fx{

void* action_callback_stub(void*){ return nullptr; }
    
    
Action::Callback Action::callbackStub()
{
    return action_callback_stub;
}
    
}//namespace r64fx
