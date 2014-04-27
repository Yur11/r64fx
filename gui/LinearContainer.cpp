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
    float y = paddingTop();
    for(auto ch : children)
    {
        ch->setY(y);
        y += ch->height();
        if(w < ch->width())
            w = ch->width();
    }
    
    setWidth(w + paddingWidth());
    setHeight(y + paddingBottom());
}


void LinearContainer::alignHorizontally()
{
    float h = 0.0;
    float x = paddingLeft();
    for(auto ch: children)
    {
        ch->setX(x);
        x += ch->width();
        if(h < ch->height())
            h = ch->height();
    }
    
    setWidth(x + paddingRight());
    setHeight(h + paddingHeight());
}
 
}//namespace r64fx