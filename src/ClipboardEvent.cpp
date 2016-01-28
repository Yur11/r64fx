#include "ClipboardEvent.hpp"

namespace r64fx{

void ClipboardDataTransmitEvent::transmit(void* data, int size)
{
    m_data[0] = data;
    m_size[0] = size;
}

}//namespace r64fx