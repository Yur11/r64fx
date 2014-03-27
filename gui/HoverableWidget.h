#ifndef R64FX_HOVERABLE_WIDGET_H
#define R64FX_HOVERABLE_WIDGET_H

#include "Widget.h"

namespace r64fx{
    
class View;
    
/** @brief Base class for widgets that can be highlighted on mouse hover. */
class HoverableWidget : public Widget{
    static HoverableWidget* _current_hovered;
    static View* _hover_view;
    
public:
    inline static void reset() { _current_hovered = nullptr; }

    inline static View* hoverView() { return _hover_view; }
    
    HoverableWidget(Widget* parent = nullptr) : Widget(parent) {}
    
    ~HoverableWidget() {}
    
    inline bool isHovered() { return _current_hovered == this; }
    
    bool isHoveredInView(View* view);
    
    inline bool shouldBeRenderedAsHovered() { return isHoveredInView(_hover_view); }
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void mouseEnterEvent(MouseEvent* event);
    
    virtual void mouseLeaveEvent(MouseEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_HOVERABLE_WIDGET_H