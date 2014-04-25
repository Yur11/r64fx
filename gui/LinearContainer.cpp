#include "LinearContainer.h"

#include <iostream>
using namespace std;

namespace r64fx{
    
LinearContainer::LinearContainer(Widget* parent)
:Widget(parent)
{
}


void LinearContainer::alignVertically()
{
    float w = 0.0;
    float y = 0.0;
    for(auto ch : _children)
    {
        ch->setY(y);
        y += ch->height();
        if(w < ch->width())
            w = ch->width();
    }
    
    setWidth(w);
}


void LinearContainer::alignHorizontally()
{
    float h = 0.0;
    float x = 0.0;
    for(auto ch: _children)
    {
        ch->setX(x);
        x += ch->width();
        if(h < ch->height())
            h = ch->height();
    }
    
    setHeight(h);
}
 
}//namespace r64fx