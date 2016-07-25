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
    std::string m_text = "";
    void (*m_on_value_changed)(void* arg, Widget_Knob* knob, float new_value) = nullptr;
    void* m_on_value_changed_arg = nullptr;

public:
    Widget_Knob(Widget* parent = nullptr);

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

    bool showsText(bool yes);

    bool showsText() const;

    void setText(const std::string &text);

    std::string text() const;

    void onValueChanged(void (*on_value_changed)(void* arg, Widget_Knob* knob, float new_value), void* arg = nullptr);

    virtual bool isBipolar() = 0;

    void resizeAndRealign();

protected:
    void setAnimation(KnobAnimation* animation);

    void paintAnimation(Painter* painter, int frame_num);

    void paintText(Painter* painter);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};


class Widget_UnipolarKnob : public Widget_Knob{
public:
    Widget_UnipolarKnob(Widget* parent = nullptr);

    virtual void setValue(float value, bool notify = false);

    virtual bool isBipolar();

protected:
    virtual void paintEvent(PaintEvent* event);
};


class Widget_BipolarKnob : public Widget_Knob{
    float m_mid_value = 0.5f;

public:
    Widget_BipolarKnob(Widget* parent = nullptr);

    virtual void setValue(float value, bool notify = false);

    void setMidValue(float value);

    float midValue() const;

    float lowerRange() const;

    float upperRange() const;

    virtual bool isBipolar();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_KNOB_HPP