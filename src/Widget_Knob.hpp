#ifndef R64FX_WIDGET_KNOB_HPP
#define R64FX_WIDGET_KNOB_HPP

#include "Widget.hpp"

namespace r64fx{

class KnobAnimation;

enum class KnobStyle{
    Bipolar,
    Unipolar
};

class Widget_Knob : public Widget{
protected:
    float           m_min_value = 0.0f;
    float           m_max_value = 1.0f;
    float           m_value_step = 0.005f;
    float           m_value = 0.0f;
    KnobAnimation*  m_animation = nullptr;
    void (*m_on_value_changed)(void* arg, Widget_Knob* knob, float new_value) = nullptr;
    void* m_on_value_changed_arg = nullptr;

public:
    Widget_Knob(KnobStyle style, int size, Widget* parent = nullptr);

    virtual ~Widget_Knob();

    virtual void setValue(float value, bool notify = false) = 0;

    float value() const;

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    void setValueStep(float step);

    float valueStep() const;

    float valueRange() const;

    void onValueChanged(void (*on_value_changed)(void* arg, Widget_Knob* knob, float new_value), void* arg = nullptr);

protected:
    void paintAnimation(Painter* painter, int frame_num);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_KNOB_HPP
