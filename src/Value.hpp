#ifndef R64FX_VALUE_HPP
#define R64FX_VALUE_HPP

namespace r64fx{

class Value{
    float  m_min_value   = 0.0f;
    float  m_max_value   = 1.0f;
    float  m_value_step  = 0.005f;
    float  m_value       = 0.0f;
    void (*m_on_value_changed)(void* arg, float new_value) = nullptr;
    void* m_on_value_changed_arg = nullptr;

public:
    Value();

    void setValue(float value, bool notify = false);

    float value() const;

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    void setValueStep(float step);

    float valueStep() const;

    float valueRange() const;

    void onValueChanged(void (*on_value_changed)(void* arg, float new_value), void* arg = nullptr);
};

}//namespace r64fx

#endif//R64FX_VALUE_HPP
