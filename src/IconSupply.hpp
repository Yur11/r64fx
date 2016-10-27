#ifndef R64FX_ICON_SUPPLY_HPP
#define R64FX_ICON_SUPPLY_HPP

#include "IconNames.hpp"
#include "Color.hpp"

namespace r64fx{

class Image;

struct IconColors{
    Color stroke1 = {0, 0, 0, 0};
    Color fill1   = {255, 255, 255, 0};
    Color stroke2 = {31, 31, 31, 0};
    Color fill2   = {223, 223, 223, 0};
};

bool operator!=(const IconColors &a, const IconColors &b);

inline bool operator==(const IconColors &a, const IconColors &b)
{
    return !operator!=(a, b);
}

Image* get_icon(IconName name, int size, IconColors* ic = nullptr);

void free_icon(Image* icon_image);

}//namespace r64fx

#endif//R64FX_ICON_SUPPLY_HPP
