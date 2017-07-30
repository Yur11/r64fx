#ifndef R64FX_INSTANCE_COUNTER_HPP
#define R64FX_INSTANCE_COUNTER_HPP

namespace r64fx{

class InstanceCounter{
    long m_count = 0;

public:
    void created();

    void destroyed();

private:
    virtual void initEvent();

    virtual void cleanupEvent();
};

}//namespace r64fx

#endif//R64FX_INSTANCE_COUNTER_HPP
