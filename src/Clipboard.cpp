#include "Clipboard.hpp"

namespace r64fx{

bool ClipboardDataType::isText() const
{
    return m_str.empty();
}


std::string ClipboardDataType::mimeType() const
{
    return m_str;
}


ClipboardMetadata::ClipboardMetadata(const ClipboardDataType &type, Point<int> position)
{

}


ClipboardMetadata::ClipboardMetadata(const std::vector<ClipboardDataType> &types, Point<int> position)
{

}


const std::vector<ClipboardDataType>& ClipboardMetadata::types() const
{
    return m_types;
}


Point<int> ClipboardMetadata::position() const
{
    return m_position;
}

}//namespace r64fx