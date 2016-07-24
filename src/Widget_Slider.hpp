#ifndef R64FX_WIDGET_SLIDER_HPP
#define R64FX_WIDGET_SLIDER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_Slider : public Widget{
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value = 0.0f;

public:
    Widget_Slider(int length, Orientation orientation, Widget* parent = nullptr);

    virtual ~Widget_Slider();

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    float valueRange() const;

    void setValue(float value);

    float value() const;

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_SLIDER_HPP