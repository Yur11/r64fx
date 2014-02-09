#ifndef R64FX_GUI_ICON_H
#define R64FX_GUI_ICON_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
class Icon{
    Texture _texture;
    
public:
    Size<float> size;
    
    Icon(Size<float> size = Size<float>(32, 32), Texture texture = Texture::badTexture())
    : _texture(texture)
    , size(size)
    {}
    
    Icon(Size<float> size, std::string tex_file)
    : Icon(size, Texture(tex_file))
    {}

    void render();
    
    static Icon defaultIcon(Size<float> size = Size<float>(18, 18));
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H