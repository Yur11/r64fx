#include "ClipboardEvent.hpp"
#include <cstring>

namespace r64fx{

void ClipboardDataTransmitEvent::transmit(void* data, int size)
{
    if(data != nullptr && size > 0)
    {
        m_data[0] = new char[size];
        m_size[0] = size;
        memcpy(m_data[0], data, size);
    }
}

}//namespace r64fx