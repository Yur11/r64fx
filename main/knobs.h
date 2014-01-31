#ifndef R64FX_KNOBS_H
#define R64FX_KNOBS_H

#include "gui/Widget.h"
#include "gui/rotations.h"
#include "gui/Texture.h"


namespace r64fx{

    
/** @brief Base class for knob widgets. */    
class BasicKnob : public Widget, public Padding{
    
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
    
    Message value_changed;
    
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
    
    virtual void render(RenderingContextId_t context_id)
    {
        background->render(context_id, Widget::rect());
        handle->render(context_id, Widget::rect(), angle, radius);
    }
};


class TexturedKnobBackground{
    Texture _tex;
    
public:
    TexturedKnobBackground(Texture tex);
    TexturedKnobBackground(std::string tex) : TexturedKnobBackground(Texture(tex)) {}
    
    void render(RenderingContextId_t context_id, Rect<float> rect);
};


class KnobHandleTypeA{
public:
    static void init();
    
    void render(RenderingContextId_t context_id, Rect<float> rect, float angle, float radius);
};

    
}//namespace r64fx

#endif//R64FX_KNOBS_H