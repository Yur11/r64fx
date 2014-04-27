#ifndef R64FX_GUI_VIEW_H
#define R64FX_GUI_VIEW_H

#include "Widget.h"

namespace r64fx{
    
class View : public Widget{
    Point<float> _offset;
    Rect<float> scroll_limits;
    
public:
    View(Widget* parent = nullptr);
    
    inline Point<float> offset() const { return _offset; }
    
    void setOffset(Point<float> offset);
    
    inline void setOffset(float x, float y) { setOffset({ x, y }); }
    
    inline Rect<float> scrollLimits() const { return scroll_limits; }
    
    void setScrollLimits(Rect<float> limits);
    
    void dropScrollLimits();
    void dropLeftScrollLimit();
    void dropTopScrollLimit();
    void dropRightScrollLimit();
    void dropBottomScrollLimit();
    
    /** @brief Draw the widget and it's visible children. */
//     virtual void render();
    
    /** @brief Update the list of visible widgets. */
//     virtual void clip(Rect<float> rect);
    
    /** @brief Recursivly calculate window coordinates for the widget tree. */
    virtual void project(Point<float> p);
    
    /** @brief Recursivly (re)upload data to video memory for all visible widgets. */
//     virtual void updateVisuals();
};
    
}//namespace r64fx

#endif//R64FX_GUI_VIEW_H