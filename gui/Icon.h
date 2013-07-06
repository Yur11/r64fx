#ifndef R64FX_GUI_ICON_H
#define R64FX_GUI_ICON_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
class Icon{
    Texture* _texture;

public:
    Size<float> size;

    Icon(Size<float> size = {32, 32}, Texture* texture = nullptr)
    : _texture(texture)
    {}

    inline bool isOk() const { return _texture != nullptr; }

    void render();

    static Icon find(std::string name);

    static Icon defaultIcon();
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H