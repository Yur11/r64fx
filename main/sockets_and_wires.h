#ifndef R64FX_SOCKETS_AND_WIRES_H
#define R64FX_SOCKETS_AND_WIRES_H

#include "Widget.h"

namespace r64fx{
    
/** @brief Base class for sockets that can be connected with wires. */
class Socket : public Widget{
public:
    Socket(Widget* parent = nullptr);
};


/** @brief  */
class Wire{
    Socket* a = nullptr;
    Socket* b = nullptr;
    
};
    
}//namespace r64fx

#endif//R64FX_SOCKETS_AND_WIRES_H