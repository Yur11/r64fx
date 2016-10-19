#ifndef R64FX_WIDGET_SLIDER_HPP
#define R64FX_WIDGET_SLIDER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_Slider : public Widget{
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value = 0.0f;
    void (*m_on_value_changed)(void* arg, Widget_Slider* slider, float value) = nullptr;
    void* m_on_value_changed_arg = nullptr;

public:
    Widget_Slider(int length, Orientation orientation, Widget* parent = nullptr);

    virtual ~Widget_Slider();

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    float valueRange() const;

    void setValue(float value);

    void setValue(Point<int> position);

    float value() const;

    int barLength() const;

    int barOffset() const;

    bool barVisible(bool yes);

    bool barVisible() const;

    bool isFlipped(bool yes);

    bool isFlipped() const;

    bool isReversed(bool yes);

    bool isReversed() const;

    void onValueChanged(void (on_value_changed)(void* arg, Widget_Slider* slider, float value), void* arg = nullptr);

protected:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_SLIDER_HPP
