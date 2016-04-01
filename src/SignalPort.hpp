#ifndef R64FX_SIGNAL_PORT_HPP
#define R64FX_SIGNAL_PORT_HPP

#include <string>

namespace r64fx{

class SignalPort{
    float* m_buffer = nullptr;
    std::string m_name = "";
    float m_default_value = 0.0f;

protected:
    SignalPort(const std::string name = "", float default_value = 0.0f, int size = 0);

public:
    ~SignalPort();

    void resize(int size);

    void free();

    float &slot(int i) const;

    float* buffer() const;

    inline void setDefautValue(float value)
    {
        m_default_value = value;
    }

    inline float defaultValue() const
    {
        return m_default_value;
    }
};


class SignalSink : public SignalPort{
public:
    SignalSink(const std::string name = "", float default_value = 0.0f, int size = 0);
};


class SignalSource : public SignalPort{
public:
    SignalSource(const std::string name = "", float default_value = 0.0f, int size = 0);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_PORT_HPP