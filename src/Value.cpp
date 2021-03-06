#include "Value.hpp"

namespace r64fx{

namespace{
    void on_value_changed_stub(void* arg, float new_value) {}
}//namespace


Value::Value()
{
}


bool Value::changeValue(float value)
{
    float old_value = m_value;
    m_value = value;
    if(m_value < minValue())
        m_value = minValue();
    else if(m_value > maxValue())
        m_value = maxValue();
    return old_value != value;
}


float Value::value() const
{
    return m_value;
}


float Value::normalizedValue(float value) const
{
    return (value - minValue()) / valueRange();
}


float Value::normalizedValue() const
{
    return normalizedValue(value());
}


void Value::setMinValue(float value)
{
    m_min_value = value;
}


float Value::minValue() const
{
    return m_min_value;
}


void Value::setMaxValue(float value)
{
    m_max_value = value;
}


float Value::maxValue() const
{
    return m_max_value;
}


void Value::setValueStep(float step)
{
    m_value_step = step;
}


float Value::valueStep() const
{
    return m_value_step;
}


float Value::valueRange() const
{
    return maxValue() - minValue();
}


ValueChangeCallback::ValueChangeCallback()
{
    onValueChanged(nullptr);
}


void ValueChangeCallback::onValueChanged(void (*on_value_changed)(void* arg, float new_value), void* arg)
{
    if(on_value_changed)
    {
        m_on_value_changed = on_value_changed;
    }
    else
    {
        m_on_value_changed = on_value_changed_stub;
    }
    m_on_value_changed_arg = arg;
}


void ValueChangeCallback::valueChanged(float value)
{
    m_on_value_changed(m_on_value_changed_arg, value);
}

}//namespace r64fx
