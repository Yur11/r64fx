#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"

namespace r64fx{

class ControlAnimation;

class Widget_Control : public Widget{

public:
    Widget_Control(Widget* parent = nullptr);

    ~Widget_Control();

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    void mousePressEvent(MousePressEvent* event);

    void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP