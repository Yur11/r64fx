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
    for(auto &ch : children)
    {
        ch.rect.set_y(y);
        y += ch.rect.height();
        if(w < ch.rect.width())
            w = ch.rect.width();
    }
    
    setWidth(w + paddingWidth());
    setHeight(y + paddingBottom());
}


void LinearContainer::alignHorizontally()
{
    float h = 0.0;
    float x = paddingLeft();
    for(auto &ch: children)
    {
        ch.rect.set_x(x);
        x += ch.width();
        if(h < ch.height())
            h = ch.height();
    }
    
    setWidth(x + paddingRight());
    setHeight(h + paddingHeight());
}


void LinearContainer::projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect)
{
    Widget::projectToRootAndClipVisible(parent_position, parent_visible_rect);
    
//     auto it = visibleChildren().begin();
//     if(it == allChildren().begin();)
}
 
}//namespace r64fx