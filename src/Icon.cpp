#include "Icon.hpp"
#include <map>
#include <iostream>
#include "Painter.hpp"


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

Painter* Icon::p;

Size<float> Icon::default_size = { 16, 16 };

void Icon::render()
{
    if(texture)
    {           
        float pos[6] = {
            size.w, 0.0,
            0.0, size.h,
            size.w, size.h
        };
        
        p->bindBuffer();
        p->setPositions(pos, 6, 2);
        p->unbindBuffer();
        
//         Painter::useCurrent2dProjection();
        Painter::setColor(color);
        Painter::setTexture(texture->glName());
        
        p->bindArray();
        p->render(GL_TRIANGLE_STRIP);
        p->unbindArray();        
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

    
}//namespace r64fx
