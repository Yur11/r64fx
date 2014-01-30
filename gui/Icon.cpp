#include "Icon.h"
#include <map>
#include <iostream>
#include "TexturedRect.h"


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

void Icon::render()
{
    if(_texture.isGood())
    {        
        TexturedRect::render(0.0, 0.0, size.w, size.h, 0.0, 0.0, 1.0, 1.0, _texture.id());
    }
}

Icon Icon::defaultIcon(Size<float> size)
{
    return Icon(size, Texture::badTexture());
}

    
}//namespace r64fx
