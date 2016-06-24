#ifndef R64FX_CLIPBOARD_EVENT_HPP
#define R64FX_CLIPBOARD_EVENT_HPP

#include "Clipboard.hpp"

namespace r64fx{

class Widget;
class Window;

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


class ClipboardDataTransmitEvent : public ClipboardDataEvent{
    void (*m_on_transmit)(Window* window, void* data, int size);
    Window* m_window;

public:
    ClipboardDataTransmitEvent(
        ClipboardMode mode, ClipboardDataType type,
        void (on_transmit)(Window* window, void* data, int size),
        Window* window
    )
    : ClipboardDataEvent(mode, type)
    , m_on_transmit(on_transmit)
    , m_window(window)
    {}

    void transmit(void* data, int size);
};


class ClipboardMetadataRecieveEvent : public ClipboardEvent{
    const ClipboardMetadata &m_metadata;

public:
    ClipboardMetadataRecieveEvent(ClipboardMode mode, const ClipboardMetadata &metadata)
    : ClipboardEvent(mode)
    , m_metadata(metadata)
    {}

    inline const ClipboardMetadata &metadata() const { return m_metadata; }

    inline bool has(const ClipboardDataType &type) const
    {
        return m_metadata.has(type);
    }
};


class DndMetadataEvent{
    const ClipboardMetadata &m_metadata;

public:
    DndMetadataEvent(const ClipboardMetadata &metadata)
    : m_metadata(metadata)
    {}

    inline const ClipboardMetadata& metadata() const
    {
        return m_metadata;
    }

    inline bool has(const ClipboardDataType &type) const
    {
        return m_metadata.has(type);
    }
};


class DndMoveEvent : public DndMetadataEvent{
    int mx = 0;
    int my = 0;
    bool &m_accepted;

public:
    DndMoveEvent(int x, int y, const ClipboardMetadata &metadata, bool &accepted)
    : DndMetadataEvent(metadata)
    , mx(x)
    , my(y)
    , m_accepted(accepted)
    {}

    inline int x() const { return mx; }
    inline int y() const { return my; }

    inline void accept()
    {
        m_accepted = true;
    }

    inline void reject()
    {
        m_accepted = false;
    }
};


class DndDropEvent : public DndMetadataEvent{
    ClipboardDataType &m_data_type;
    bool &m_accepted;

public:
    DndDropEvent(const ClipboardMetadata &metadata, ClipboardDataType &out_data_type, bool &out_accepted)
    : DndMetadataEvent(metadata)
    , m_data_type(out_data_type)
    , m_accepted(out_accepted)
    {}

    inline void accept(const ClipboardDataType &data_type)
    {
        m_data_type = data_type;
        m_accepted = true;
    }

    inline void reject()
    {
        m_accepted = false;
    }
};


class DndLeaveEvent{

};


class DndFinishedEvent{
    Widget* m_dnd_object = nullptr;

public:
    DndFinishedEvent(Widget* dnd_object)
    : m_dnd_object(dnd_object)
    {}

    inline Widget* dndObject() const { return m_dnd_object; }
};

}//namespace r64fx

#endif//R64FX_CLIPBOARD_EVENT_HPP