#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"

namespace r64fx{

enum class ControlType{
    UnipolarRadius,
    BipolarRadius,
    UnipolarSector,
    BipolarSector,
    VerticalFader,
    HorizontalFader
};


class ControlAnimation{

};


class Widget_Control : public Widget{
    ControlAnimation*  m_animation = nullptr;
    int                m_position  = 0;

public:
    Widget_Control(ControlType type, Size<int> size, Widget* parent = nullptr);

    ~Widget_Control();

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    void mousePressEvent(MousePressEvent* event);

    void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP