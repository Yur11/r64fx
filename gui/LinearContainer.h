#ifndef R64FX_LINEAR_CONTAINER_H
#define R64FX_LINEAR_CONTAINER_H

#include "Widget.h"
#include "Padding.h"

namespace r64fx{
    
class LinearContainer : public Widget, public Padding<float>{
    
public:
    LinearContainer(Widget* parent = nullptr);
    
    void alignVertically();
    
    void alignHorizontally();
    
    void projectToRootAndClipVisible(Rect<float> rect);
};
    
}//namespace r64fx

#endif//R64FX_LINEAR_CONTAINER_H