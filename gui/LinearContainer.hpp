#ifndef R64FX_LINEAR_CONTAINER_H
#define R64FX_LINEAR_CONTAINER_H

#include "Widget.hpp"
#include "Padding.hpp"

namespace r64fx{
    
class LinearContainer : public Widget, public Padding<float>{
    
public:
    LinearContainer(Widget* parent = nullptr);
    
    void alignVertically();
    
    void alignHorizontally();
    
    void projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect);
};
    
}//namespace r64fx

#endif//R64FX_LINEAR_CONTAINER_H