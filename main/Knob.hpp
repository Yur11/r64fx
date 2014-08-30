#ifndef R64FX_MAIN_KNOB_H
#define R64FX_MAIN_KNOB_H

#include "gui/Widget.hpp"
#include "gui/Painter.hpp"
#include "gui/Texture.hpp"
#include "gui/Padding.hpp"


namespace r64fx{

    
/** @brief Base class for knob widgets. */    
class BasicKnob : public Widget, public Padding<float>{
            
public:    
    BasicKnob(Widget* parent = nullptr);
    
    float min_angle = 25;
    float max_angle = 335;
    
    float min_value = 0.0;
    float max_value = 1.0;
    
    float angle = 25;
    
    inline float value()
    {
        return (max_value - min_value) * (angle - min_angle) / (max_angle - min_angle) + min_value;
    }
    
protected:
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};



class Knob : public BasicKnob{
    Painter p;
    Texture2D* background_tex = nullptr;
    Texture2D* rotatable_tex = nullptr;
    Texture2D* foreground_tex = nullptr;
    
public:
    Knob(Texture2D* background, Texture2D* rotatable, Texture2D* foreground = nullptr);
    
    Knob(std::string background, std::string rotatable, std::string foreground = "");

protected:
    virtual void appearanceChangeEvent();
    
    virtual void render();
};

    
}//namespace r64fx

#endif//R64FX_KNOBS_H