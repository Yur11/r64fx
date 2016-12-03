#ifndef R64FX_THREAD_OBJECT_MESSAGE_HPP
#define R64FX_THREAD_OBJECT_MESSAGE_HPP

namespace r64fx{

class ThreadObjectMessage{
    unsigned long m_key     = 0;
    unsigned long m_value   = 0;

public:
    ThreadObjectMessage(unsigned long key, unsigned long value)
    : m_key(key)
    , m_value(value)
    {

    }

    ThreadObjectMessage(unsigned long key, void* value)
    : m_key(key)
    , m_value((unsigned long)value)
    {

    }

    ThreadObjectMessage(unsigned long key)
    : m_key(key)
    , m_value(0)
    {

    }

    ThreadObjectMessage() {}


    inline unsigned long key() const
    {
        return m_key;
    }

    inline unsigned long value() const
    {
        return m_value;
    }
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_MESSAGE_HPP
