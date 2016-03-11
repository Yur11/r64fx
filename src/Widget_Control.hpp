#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"

namespace r64fx{

enum class ControlType{
    UnipolarRadius,
    BipolarRadius,
    UnipolarSector,
    BipolarSector,
    VerticalSlider,
    HorizontalSlider
};


class ControlAnimation{
    int m_min_position = 0;
    int m_max_position = 255;

    float m_min_value = 0.0f;
    float m_max_value = 1.0f;

public:
    inline void setMinPosition(int pos) { m_min_position = pos; }
    inline void setMaxPosition(int pos) { m_max_position = pos; }

    inline int minPosition() const { return m_min_position; }
    inline int maxPosition() const { return m_max_position; }

    inline int positionRange() const { return maxPosition() - minPosition() + 1; }

    inline int boundPosition(int pos) const
    {
        if(pos < minPosition())
            return minPosition();
        else if(pos > maxPosition())
            return maxPosition();
        else
            return pos;
    }

    inline void setMinValue(int val) { m_min_value = val; }
    inline void setMaxValue(int val) { m_max_value = val; }

    inline int minValue() const { return m_min_value; }
    inline int maxValue() const { return m_max_value; }

    inline int valueRange() const { return maxValue() - minValue(); }
};


class Widget_Control : public Widget{
    ControlAnimation*  m_animation = nullptr;
    int                m_position  = 0;
    void             (*m_on_value_changed)(Widget_Control*, void*);
    void*              m_on_value_changed_data = nullptr;

public:
    Widget_Control(ControlType type, int size, Widget* parent = nullptr);

    ~Widget_Control();

    void setValue(float value);

    float value() const;

    void onValueChanged(void (*callback)(Widget_Control*, void*), void* data = nullptr);

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP