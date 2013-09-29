#ifndef R64FX_KNOB_H
#define R64FX_KNOB_H

#include "Widget.h"

namespace r64fx{
    
/** @brief A knob widget. */    
class Knob : public Widget, public Padding{
    float _step = 0.0;
    float prev_mouse_y = 0.0;
    
public:
    Knob(Widget* parent = nullptr);
    
    float min_angle = 45;
    float max_angle = 315;
    
    float min_value = 0.0;
    float max_value = 1.0;
    
    float angle = 0.0;
    
    inline float value()
    {
        return (max_value - min_value) * (angle - min_angle) / (max_angle - min_angle) + min_value;
    }
    
    int npoints = 12;
    
    float radius = 75.0;
    
    virtual void render();
    
    virtual void update();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_KNOB_H