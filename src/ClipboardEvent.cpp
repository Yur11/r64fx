#include "ClipboardEvent.hpp"

namespace r64fx{

void ClipboardDataTransmitEvent::transmit(void* data, int size)
{
    m_on_transmit(m_window, data, size);
}

}//namespace r64fx