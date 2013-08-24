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

    void render();

    static Icon find(std::string name, Size<float> size = Size<float>(32, 32));
    
    inline static Icon find(std::string name, float width, float height)
    {
        return Icon::find(name, Size<float>(width, height));
    }

    static Icon defaultIcon(Size<float> size = Size<float>(18, 18));
    
    static Icon loadFrom(std::vector<std::string>* data_paths);
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H