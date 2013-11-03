#ifndef R64FX_KNOBS_H
#define R64FX_KNOBS_H

#include "Widget.h"
#include "rotations.h"
#include "Texture.h"


namespace r64fx{

    
/** @brief Base class for knob widgets. */    
class BasicKnob : public Widget, public Padding{
    float prev_mouse_y = 0.0;
    
protected:
    float rad_angle = 0.0;
    
public:
    static void init();
    
    BasicKnob(Widget* parent = nullptr);
    
    float min_angle = 45;
    float max_angle = 315;
    
    float min_value = 0.0;
    float max_value = 1.0;
    
    float angle = 45;
    
    inline float value()
    {
        return (max_value - min_value) * (angle - min_angle) / (max_angle - min_angle) + min_value;
    }
    
    float radius = 50.0;
        
    virtual void update();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};


template<typename BackgroundT, typename HandleT> class Knob : public BasicKnob{
    BackgroundT* background;
    HandleT* handle;
    
public:
    Knob(BackgroundT* background, HandleT* handle)
    : background(background)
    , handle(handle)
    {}
    
    virtual void render()
    {
        background->render(Widget::rect());
        handle->render(Widget::rect(), angle, radius);
    }
};


class TexturedKnobBackground{
    Texture _tex;
    
public:
    TexturedKnobBackground(Texture tex);
    TexturedKnobBackground(std::string tex) : TexturedKnobBackground(Texture(tex)) {}
    
    void render(Rect<float> rect);
};


class KnobHandleTypeA{
public:
    static void init();
    
    void render(Rect<float> rect, float angle, float radius);
};

    
}//namespace r64fx

#endif//R64FX_KNOBS_H