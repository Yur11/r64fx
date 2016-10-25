#ifndef R64FX_ICON_SUPPLY_HPP
#define R64FX_ICON_SUPPLY_HPP

#include "IconNames.hpp"

namespace r64fx{

class Image;
    
Image* get_icon(IconName name, int size, bool highlighted = false);

void free_icon(Image* icon_image);

}//namespace r64fx

#endif//R64FX_ICON_SUPPLY_HPP
