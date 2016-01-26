#ifndef R64FX_CLIPBOARD_HPP
#define R64FX_CLIPBOARD_HPP

#include "Point.hpp"
#include <vector>

namespace r64fx{

enum class ClipboardMode{
    Clipboard,
    Selection,
    DragAndDrop
};


class ClipboardDataType{
    const char* m_str;

public:
    ClipboardDataType(const char* str = "") : m_str(str) {}

    inline const char* str() const { return m_str; }
};


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