#ifndef R64FX_CLIPBOARD_EVENT_HPP
#define R64FX_CLIPBOARD_EVENT_HPP

#include "Clipboard.hpp"

namespace r64fx{

class ClipboardEvent{
    ClipboardMode     m_mode = ClipboardMode::Bad;

public:
    ClipboardEvent(ClipboardMode mode)
    : m_mode(mode)
    {}

    inline ClipboardMode mode() const { return m_mode; }

};


class ClipboardDataEvent : public ClipboardEvent{
    ClipboardDataType m_type;

public:
    ClipboardDataEvent(ClipboardMode mode, ClipboardDataType type)
    : ClipboardEvent(mode)
    , m_type(type)
    {}

    inline ClipboardDataType type() const { return m_type; }
};


class ClipboardDataRecieveEvent : public ClipboardDataEvent{
    void* m_data = nullptr;
    int   m_size = 0;

public:
    ClipboardDataRecieveEvent(ClipboardMode mode, ClipboardDataType type, void* data, int size)
    : ClipboardDataEvent(mode, type)
    , m_data(data)
    , m_size(size)
    {}

    inline void* data() const { return m_data; }

    inline int size() const { return m_size; }
};


class ClipboardDataTransmitEvent : ClipboardDataEvent{
    void** m_data = nullptr;
    int*   m_size = nullptr;

public:
    ClipboardDataTransmitEvent(ClipboardMode mode, ClipboardDataType type, void** data, int* size)
    : ClipboardDataEvent(mode, type)
    , m_data(data)
    , m_size(size)
    {}

    void transmit(void* data, int size);
};


class ClipboardMetadataRecieveEvent : ClipboardEvent{
    const ClipboardMetadata &m_metadata;

public:
    ClipboardMetadataRecieveEvent(ClipboardMode mode, const ClipboardMetadata &metadata)
    : ClipboardEvent(mode)
    , m_metadata(metadata)
    {}

    inline const ClipboardMetadata &metadata() const { return m_metadata; }
};

}//namespace r64fx

#endif//R64FX_CLIPBOARD_EVENT_HPP