#include "SignalNodeConnection.hpp"
#include "SignalNode.hpp"
#include <algorithm>

using namespace std;

namespace r64fx{

SignalNodeConnection::SignalNodeConnection(SignalNode* dst, SignalSink* dst_port, SignalNode* src, SignalPort* src_port)
: m_dst(dst)
, m_src(src)
, m_dst_port(dst_port)
, m_src_port(src_port)
{

}


void SignalNodeConnection::process()
{
    switch(m_relation_type)
    {
        case RelationType::Matching:
        {
            int slot_count = min(m_dst->slotCount(), m_src->slotCount());
            for(int i=0; i<slot_count; i++)
            {
                m_dst_port->slot(m_dst->slotOffset() + i) = m_src_port->slot(m_src->slotOffset() + i);
            }
            break;
        }

        default:
            break;
    }
}

SignalNodeConnection::RelationType SignalNodeConnection::relationType() const
{
    return m_relation_type;
}

}//namespace r64fx