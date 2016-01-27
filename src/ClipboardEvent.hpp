#ifndef R64FX_CLIPBOARD_EVENT_HPP
#define R64FX_CLIPBOARD_EVENT_HPP

#include "Clipboard.hpp"

namespace r64fx{

class ClipboardEvent{
    ClipboardMode     m_mode = ClipboardMode::Bad;
    ClipboardDataType m_type;

public:
    ClipboardEvent(ClipboardMode mode, ClipboardDataType type)
    : m_mode(mode)
    , m_type(type)
    {}

    inline ClipboardMode mode() const { return m_mode; }

    inline ClipboardDataType type() const { return m_type; }
};


class ClipboardDataRecieveEvent : public ClipboardEvent{
    void* m_data = nullptr;
    int   m_size = 0;

public:
    ClipboardDataRecieveEvent(ClipboardMode mode, ClipboardDataType type, void* data, int size)
    : ClipboardEvent(mode, type)
    , m_data(data)
    , m_size(size)
    {}

    inline void* data() const { return m_data; }

    inline int size() const { return m_size; }
};


class ClipboardDataTransmitEvent : ClipboardEvent{
    void** m_data = nullptr;
    int*   m_size = nullptr;

public:
    ClipboardDataTransmitEvent(ClipboardMode mode, ClipboardDataType type, void** data, int* size)
    : ClipboardEvent(mode, type)
    , m_data(data)
    , m_size(size)
    {}

    void transmit(void* data, int size);
};


class ClipboardMetadataRecieveEvent{

};

}//namespace r64fx

#endif//R64FX_CLIPBOARD_EVENT_HPP