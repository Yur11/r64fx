#include "Icon.h"
#include <map>
#include <iostream>
#include "RectPainter.h"


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

void Icon::render()
{
    if(_texture.isGood())
    {        
        RectPainter::prepare();
        RectPainter::setTexCoords(0.0, 0.0, 1.0, 1.0);
        RectPainter::setTexture(_texture.id());
        RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
        RectPainter::setCoords(0.0, 0.0, size.w, size.h);
        RectPainter::render();
    }
}

Icon Icon::defaultIcon(Size<float> size)
{
    return Icon(size, Texture::badTexture());
}

    
}//namespace r64fx
