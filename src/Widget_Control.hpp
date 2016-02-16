#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_Control : public Widget{
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value     = 0.0f;
    float m_range_rcp = 1.0f;
    void* m_on_value_changed_data = nullptr;
    void(*m_on_value_changed)(Widget_Control* widget, void* data) = nullptr;
    Point<int> m_mouse_pos;

public:
    Widget_Control(Widget* parent = nullptr);

    ~Widget_Control();

    float minValue() const;

    float maxValue() const;

    void setValueRange(float minval, float maxval);

    float value() const;

    void onValueChanged(void (*callback)(Widget_Control*, void*), void* data = nullptr);

protected:
    void mousePressEvent(MousePressEvent* event);

    void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP