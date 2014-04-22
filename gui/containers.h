#ifndef R64FX_GUI_CONTANERS_WIDGETS_H
#define R64FX_GUI_CONTANERS_WIDGETS_H

#include "Widget.h"
#include "Padding.h"
#include <vector>

namespace r64fx{
    

class Container : public Widget, public Padding{
    float _spacing = 0.0;
    
public:
    Container(Widget* parent = nullptr) : Widget(parent) {}
    
    /** @brief Resize the container and reposition the contents. */
    virtual void update() = 0;
    
    inline float spacing() const { return _spacing; }
    inline void setSpacing(float spacing) { _spacing = spacing; }
};
    
    
class HorizontalContainer : public Container{
public:
    HorizontalContainer(Widget* parent = nullptr) : Container(parent) {}
    
    /** @brief Resize the container and reposition the contents. */
    virtual void update();
};


class VerticalContainer : public Container{
public:
    VerticalContainer(Widget* parent = nullptr) : Container(parent) {}
    
    /** @brief Resize the container and reposition the contents. */
    virtual void update();
};
    
}//namespace r64fx

#endif//R64FX_GUI_CONTANERS_WIDGETS_H