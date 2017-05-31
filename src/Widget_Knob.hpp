#ifndef R64FX_WIDGET_KNOB_HPP
#define R64FX_WIDGET_KNOB_HPP

#include "Widget.hpp"

namespace r64fx{

class KnobAnimation;

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
    Widget_Knob(int size, Widget* parent = nullptr);

    virtual ~Widget_Knob();

    void setValue(float value, bool notify = false);

    float value() const;

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    void setValueStep(float step);

    float valueStep() const;

    float valueRange() const;

    void onValueChanged(void (*on_value_changed)(void* arg, Widget_Knob* knob, float new_value), void* arg = nullptr);

    static void debugPaint(Painter* painter, Point<int> position, int size);

private:
    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event);

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event);

    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_KNOB_HPP
