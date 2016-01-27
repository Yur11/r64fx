#ifndef R64FX_CLIPBOARD_HPP
#define R64FX_CLIPBOARD_HPP

#include "Point.hpp"
#include <vector>

namespace r64fx{

enum class ClipboardMode{
    Bad,
    Clipboard,
    Selection,
    DragAndDrop
};


class ClipboardDataType{
    const char* m_name;

public:
    ClipboardDataType(const char* name = "") : m_name(name) {}

    inline const char* name() const { return m_name; }
};


bool operator==(ClipboardDataType a, ClipboardDataType b);


class ClipboardMetadata : public std::vector<ClipboardDataType>{

public:
    ClipboardMetadata(const std::vector<ClipboardDataType> &types)
    : std::vector<ClipboardDataType>(types)
    {}

    ClipboardMetadata(ClipboardDataType type)
    : std::vector<ClipboardDataType>{type}
    {}

    ClipboardMetadata()
    {}
};


}//namespace r64x

#endif//R64FX_CLIPBOARD_EVENT_HPP