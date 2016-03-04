#ifndef R64FX_WIDGET_VIEW_HPP
#define R64FX_WIDGET_VIEW_HPP

#include "Widget.hpp"
#include "Offset.hpp"

namespace r64fx{

class Widget_View : public Widget, public Offset<int>{

public:
    Widget_View(Widget* parent = nullptr);

    ~Widget_View();

protected:
    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_VIEW_HPP