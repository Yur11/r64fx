#include "Message.h"

namespace r64fx{
    
void* dummy_callback(void*, void*)
{
    return nullptr;
}

    
Message::Message() { clear(); }


void Message::clear()
{
    callback = dummy_callback;
    data = nullptr;
}
    
}