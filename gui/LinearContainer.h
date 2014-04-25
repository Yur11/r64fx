#ifndef R64FX_LINEAR_CONTAINER_H
#define R64FX_LINEAR_CONTAINER_H

#include "Widget.h"

namespace r64fx{
    
class LinearContainer : public Widget{
    
public:
    LinearContainer(Widget* parent = nullptr);
    
    void alignVertically();
    
    void alignHorizontally();
};
    
}//namespace r64fx

#endif//R64FX_LINEAR_CONTAINER_H