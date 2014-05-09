#ifndef R64FX_MAIN_SLIDER_H
#define R64FX_MAIN_SLIDER_H

#include "gui/Widget.h"
#include "gui/Orientation.h"
#include "gui/Painter.h"
#include "gui/Texture.h"

namespace r64fx{
   
class Slider : public Widget{
    Painter p;
    Texture2D* backround_tex = nullptr;
    Texture2D* handle_tex = nullptr;
    
    bool mouse_is_down = false;
    
    float coord2value_coeff = 1.0;
    float coord2value_offset = 0.0;
        
    float coord2value(float coord);
    
    float pos2value(Point<float> p);
    
public:
    Slider(Texture2D* backround_tex, Texture2D* handle_tex, Widget* parent = nullptr);
    
    
    Slider(std::string backround_tex, std::string handle_tex, Widget* parent = nullptr)
    : Slider(
        (Texture2D*) Texture::find(backround_tex), 
        (Texture2D*) Texture::find(handle_tex),
        parent
    ) {}
    
    Orientation orientation = Orientation::Vertical;
    
    float prev_value = 1.0;
    float value = 1.0;

    float handle_rect_w_ratio = 0.5;
    float handle_rect_h_ratio = 0.24;
    
protected:
    virtual void appearanceChangeEvent();
    
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent* event);

    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_MAIN_SLIDER_H