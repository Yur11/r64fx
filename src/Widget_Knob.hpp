#ifndef R64FX_WIDGET_KNOB_HPP
#define R64FX_WIDGET_KNOB_HPP

#include "Widget.hpp"

namespace r64fx{

class KnobAnimation;

class Widget_Knob : public Widget{
protected:
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value_step = 0.005f;
    float m_value = 0.0f;
    KnobAnimation* m_animation = nullptr;

public:
    Widget_Knob(Widget* parent = nullptr);

    virtual void setValue(float value) = 0;

    float value() const;

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    void setValueStep(float step);

    float valueStep() const;

    float valueRange() const;

protected:
    void paintAnimation(Painter* painter, int frame_num);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};


class Widget_UnipolarKnob : public Widget_Knob{
public:
    Widget_UnipolarKnob(Widget* parent = nullptr);

    virtual void setValue(float value);

protected:
    virtual void paintEvent(PaintEvent* event);
};


class Widget_BipolarKnob : public Widget_Knob{
    float m_mid_value = 0.5f;

public:
    Widget_BipolarKnob(Widget* parent = nullptr);

    virtual void setValue(float value);

    void setMidValue(float value);

    float midValue() const;

    float lowerRange() const;

    float upperRange() const;

protected:
    virtual void paintEvent(PaintEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_KNOB_HPP