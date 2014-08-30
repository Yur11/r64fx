#include "CallbackList.hpp"

namespace r64fx{
    
void CallbackList::exec()
{
    for(auto c : *this)
        c.exec();
}
    
}//namespace r64fx