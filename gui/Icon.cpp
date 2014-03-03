#include "Icon.h"
#include <map>
#include <iostream>
#include "Painter.h"
#include "RectVertices.h"


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

PainterVertices* Icon::pv;


void Icon::render()
{
    if(_texture.isGood())
    {           
        float pos[6] = {
            size.w, 0.0,
            0.0, size.h,
            size.w, size.h
        };
        
        pv->bindBuffer();
        pv->setPositions(pos, 6, 2);
        pv->unbindBuffer();
        
        Painter::enable();
        Painter::useCurrent2dProjection();
        Painter::setColor(color);
        Painter::setTexture(_texture.id());
        
        pv->bindArray();
        pv->render(GL_TRIANGLE_STRIP);
        pv->unbindArray();
        
        Painter::disable();
    }
}


void Icon::init()
{
    pv = new PainterVertices(4);
    
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
    
    pv->bindBuffer();
    pv->setData(data);
    pv->unbindBuffer();
}


void Icon::cleanup()
{
    pv->deleteLater();
}


Icon Icon::defaultIcon(Size<float> size)
{
    return Icon(size, Texture::badTexture());
}

    
}//namespace r64fx
