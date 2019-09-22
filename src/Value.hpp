#ifndef R64FX_VALUE_HPP
#define R64FX_VALUE_HPP

namespace r64fx{

class Value{
    float  m_min_value   = 0.0f;
    float  m_max_value   = 1.0f;
    float  m_value_step  = 0.005f;
    float  m_value       = 0.0f;

public:
    Value();

    /* Returns true if value has changed */
    bool changeValue(float value);

    float value() const;

    float normalizedValue(float value) const; //Between 0 and 1

    float normalizedValue() const; //Using value()

    void setMinValue(float value);

    float minValue() const;

    void setMaxValue(float value);

    float maxValue() const;

    void setValueStep(float step);

    float valueStep() const;

    float valueRange() const;
};

class ValueChangeCallback{
    void (*m_on_value_changed)(void* arg, float value) = nullptr;
    void* m_on_value_changed_arg = nullptr;

public:
    ValueChangeCallback();

    void onValueChanged(void (*on_value_changed)(void* arg, float value), void* arg = nullptr);

protected:
    void valueChanged(float value);
};

}//namespace r64fx

#endif//R64FX_VALUE_HPP
