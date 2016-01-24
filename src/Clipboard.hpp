#ifndef R64FX_CLIPBOARD_HPP
#define R64FX_CLIPBOARD_HPP

#include "Point.hpp"
#include <string>
#include <vector>

namespace r64fx{

enum class ClipboardMode{
    Clipboard,
    Selection,
    DragAndDrop
};

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


}//namespace r64x

#endif//R64FX_CLIPBOARD_EVENT_HPP