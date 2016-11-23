#ifndef R64FX_THREAD_OBJECT_MESSAGE_HPP
#define R64FX_THREAD_OBJECT_MESSAGE_HPP

namespace r64fx{

class ThreadObjectMessage{
    unsigned long m1 = 0;
    unsigned long m2 = 0;

public:
    ThreadObjectMessage(unsigned long p1, unsigned long p2)
    : m1(p1), m2(p2)
    {
    }
    
    ThreadObjectMessage(unsigned long p1, void* p2)
    : m1(p1), m2((unsigned long)p2)
    {
    }

    ThreadObjectMessage()
    : m1(0), m2(0)
    {
    }
    
    inline unsigned long key() const
    {
        return m1;
    }
    
    inline unsigned long value() const
    {
        return m2;
    }
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_MESSAGE_HPP
