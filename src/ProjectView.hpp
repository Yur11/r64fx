#ifndef R64FX_PROJECT_VIEW_HPP
#define R64FX_PROJECT_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class ProjectView : public Widget{
public:
    ProjectView(Widget* parent = nullptr);

    virtual ~ProjectView();
    
public:
    virtual void paintEvent(WidgetPaintEvent* event);
    
    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_PROJECT_VIEW_HPP
