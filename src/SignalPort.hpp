#ifndef R64FX_SIGNAL_PORT_HPP
#define R64FX_SIGNAL_PORT_HPP

namespace r64fx{

class SignalPort{
    float* m_buffer = nullptr;

protected:
    SignalPort(int size = 0);

public:
    ~SignalPort();

    void resize(int size);

    void free();

    float &slot(int i) const;

    float* buffer() const;
};


class SignalSink : public SignalPort{
public:
    SignalSink(int size = 0);
};


class SignalSource : public SignalPort{
public:
    SignalSource(int size = 0);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_PORT_HPP