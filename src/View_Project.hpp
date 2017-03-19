#ifndef R64FX_VIEW_PROJECT_HPP
#define R64FX_VIEW_PROJECT_HPP

#include "Widget.hpp"

namespace r64fx{

class View_Project : public Widget{
    int mi = 0;

public:
    View_Project(Widget* parent = nullptr);

    virtual ~View_Project();
    
public:
    virtual void paintEvent(WidgetPaintEvent* event);
    
    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_VIEW_PROJECT_HPP
