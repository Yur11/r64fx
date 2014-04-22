#ifndef R64FX_MAIN_KNOB_H
#define R64FX_MAIN_KNOB_H

#include "gui/Widget.h"
#include "gui/Painter.h"
#include "gui/Texture.h"
#include "gui/Padding.h"


namespace r64fx{

    
/** @brief Base class for knob widgets. */    
class BasicKnob : public Widget, public Padding{
    
protected:
        
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
    
//     Message value_changed;
    
    float radius = 50.0;
        
    virtual void update();
        
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};


/** @brief Knob with a shiny part*/
class ShinyKnob : public BasicKnob{
    Texture2D* bg;
    Texture2D* fg;
    Texture2D* shiny;
    
    Painter p;
    
public:
    ShinyKnob(Texture2D* bg, Texture2D* fg, Texture2D* shiny);
    
//     ShinyKnob(std::string bg, std::string fg, std::string shiny):
//     ShinyKnob(Texture(bg), Texture(fg), Texture(shiny))
//     {}
    
    virtual void update();
    
    virtual void render();
};

    
}//namespace r64fx

#endif//R64FX_KNOBS_H