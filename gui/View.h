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
    
    virtual void projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect);
};
    
}//namespace r64fx

#endif//R64FX_GUI_VIEW_H