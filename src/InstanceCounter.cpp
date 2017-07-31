#include "InstanceCounter.hpp"

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG


namespace r64fx{

void InstanceCounter::created()
{
    if(m_count == 0)
        initEvent();
    m_count++;
}


void InstanceCounter::destroyed()
{
#ifdef R64FX_DEBUG
    assert(m_count > 0);
#endif//R64FX_DEBUG

    m_count--;
    if(m_count == 0)
        cleanupEvent();
}


void InstanceCounter::initEvent()
{
    
}


void InstanceCounter::cleanupEvent()
{
    
}

}//namespace r64fx
