#ifndef R64FX_CLIPBOARD_HPP
#define R64FX_CLIPBOARD_HPP

#include "Point.hpp"
#include <string>
#include <vector>


namespace r64fx{

enum class ClipboardMode{
    Bad,
    Clipboard,
    Selection,
    DragAndDrop
};


template<typename StreamT> StreamT &operator<<(StreamT &stream, ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Bad:
            stream << "Bad";
        break;

        case ClipboardMode::Clipboard:
            stream << "Clipboard";
        break;

        case ClipboardMode::Selection:
            stream << "Selection";
        break;

        case ClipboardMode::DragAndDrop:
            stream << "DragAndDrop";
        break;

        default:
        break;
    }
    return stream;
}


class ClipboardDataType{
    std::string m_name;

public:
    ClipboardDataType() : m_name("") {}

    ClipboardDataType(const std::string &name) : m_name(name) {}

    ClipboardDataType(const char* name) : m_name(name) {}

    inline bool isGood() const { return !m_name.empty(); }

    inline std::string name() const { return m_name; }
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

    ClipboardMetadata(const char* type)
    : std::vector<ClipboardDataType>{ClipboardDataType(type)}
    {}

    ClipboardMetadata()
    {}

    bool has(const ClipboardDataType &type) const;
};


}//namespace r64x

#endif//R64FX_CLIPBOARD_EVENT_HPP