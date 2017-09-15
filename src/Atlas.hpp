#ifndef R64FX_ATLAS_HPP
#define R64FX_ATLAS_HPP

#include <vector>
#include "GeometryUtils.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class AtlasItem : public Rect<short>{
    AtlasItem* m_children = nullptr;

protected:
    using Rect<short>::Rect;

    AtlasItem() {};

public:
    bool empty() const;

    bool isFull() const;

protected:
    Rect<short> allocRect(Size<short> size);

    bool freeRect(Rect<short> size);
};


class Atlas : public AtlasItem{
public:
    using AtlasItem::AtlasItem;

    Rect<short> allocRect(Size<short> size);

    bool freeRect(Rect<short> rect);
};


}//namespace r64fx

#endif//R64FX_ATLAS_HPP
