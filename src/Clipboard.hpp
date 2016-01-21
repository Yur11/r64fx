#ifndef R64FX_CLIPBOARD_HPP
#define R64FX_CLIPBOARD_HPP

#include "ClipboardMode.hpp"
#include "Point.hpp"
#include <string>
#include <vector>

namespace r64fx{

class ClipboardDataType{
    std::string m_str;

public:
    explicit ClipboardDataType(std::string str = "") : m_str(str) {}

    bool isText() const;

    std::string mimeType() const;
};


class ClipboardMetadata{
    std::vector<ClipboardDataType> m_types;
    Point<int> m_position; //For drag and drop.

public:
    ClipboardMetadata(const ClipboardDataType &type, Point<int> position = {0, 0});

    ClipboardMetadata(const std::vector<ClipboardDataType> &types, Point<int> position = {0, 0});

    const std::vector<ClipboardDataType>& types() const;

    Point<int> position() const;
};


class ClipboardDataEvent{
    void* m_data;
    int   m_size;
};


class ClipboardMetadataEvent{

};


// class ClipboardEvent{
// public:
//     enum class Type{
//         Paste,
//         Selection
//     };
//
// private:
//     std::string m_text;
//     Type        m_type;
//
// public:
//     ClipboardEvent(const std::string &text, Type type)
//     : m_text(text)
//     , m_type(type)
//     {}
//
//     inline std::string text() const { return m_text; }
//
//     inline Type type() const { return m_type; }
// };

}//namespace r64x

#endif//R64FX_CLIPBOARD_EVENT_HPP