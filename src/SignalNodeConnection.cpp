#include "SignalNodeConnection.hpp"

namespace r64fx{

SignalNodeConnection::SignalNodeConnection(float* dst, float* src)
: m_dst(dst)
, m_src(src)
{

}


void SignalNodeConnection::process()
{
    m_dst[0] = m_src[0];
}

}//namespace r64fx