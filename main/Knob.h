#ifndef R64FX_MAIN_KNOB_H
#define R64FX_MAIN_KNOB_H

#include "gui/Widget.h"
#include "gui/rotations.h"
#include "gui/Painter.h"
#include "gui/Texture.h"


namespace r64fx{

    
/** @brief Base class for knob widgets. */    
class BasicKnob : public Widget, public Padding{
    
protected:
    float rad_angle = 0.0;
        
    virtual void rotated() = 0;
    
public:    
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
    
    virtual void rotated()
    {
        handle->update(Point<float>(width() * 0.5, height() * 0.5), angle, radius);
    }
    
public:
    Knob(BackgroundT* background, HandleT* handle)
    : background(background)
    , handle(handle)
    {
        update();
        handle->update(Point<float>(width() * 0.5, height() * 0.5), angle, radius);
    }
    
    virtual void render()
    {
        background->render(Widget::rect());
        handle->render();
    }
};


class TexturedKnobBackground{
    Texture _tex;
    
public:
    TexturedKnobBackground(Texture tex);
    TexturedKnobBackground(std::string tex) : TexturedKnobBackground(Texture(tex)) {}
    
    void render(Rect<float> rect);
};


/* Rotating base + Shiny top. */
class KnobHandleTypeA{
    PainterVertices pv;
    
public:
    KnobHandleTypeA();
    
    static void init();
    
    void update(Point<float> center, float angle, float radius);
    
    void render();
};

    
}//namespace r64fx

#endif//R64FX_KNOBS_H