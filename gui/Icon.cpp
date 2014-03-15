#include "Icon.h"
#include <map>
#include <iostream>
#include "Painter.h"


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

Painter* Icon::p;


void Icon::render()
{
    if(_texture.isGood())
    {           
        float pos[6] = {
            size.w, 0.0,
            0.0, size.h,
            size.w, size.h
        };
        
        p->bindBuffer();
        p->setPositions(pos, 6, 2);
        p->unbindBuffer();
        
//         Painter::enable();
        Painter::useCurrent2dProjection();
        Painter::setColor(color);
        Painter::setTexture(_texture.id());
        
        p->bindArray();
        p->render(GL_TRIANGLE_STRIP);
        p->unbindArray();
        
//         Painter::disable();
    }
}


void Icon::init()
{
    p = new Painter(4);
    
    float data[16] = {
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p->bindBuffer();
    p->setData(data);
    p->unbindBuffer();
}


void Icon::cleanup()
{
    p->deleteLater();
}


Icon Icon::defaultIcon(Size<float> size)
{
    return Icon(size, Texture::badTexture());
}

    
}//namespace r64fx
