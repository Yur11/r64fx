#include "containers.h"
#include "MouseEvent.h"


namespace r64fx{
    
    
void HorizontalContainer::update()
{
    Widget::update();
    
    float chx = paddingLeft();
    float chy = paddingTop();
    float new_width = 0.0;
    float new_height = 0.0;
    for(auto ch : _children)
    {
        ch->setX(chx);
        ch->setY(chy);
        chx += ch->width() + spacing();
        
        new_width += ch->width();
        if(ch->height() > new_height) new_height = ch->height();
    }
    new_width += (childrenCount() - 1) * spacing();
    new_width += (paddingLeft() + paddingRight());
    new_height += (paddingTop() + paddingBottom());
    resize(new_width, new_height);
}


void VerticalContainer::update()
{
    Widget::update();
    
    float chx = paddingLeft();
    float chy = paddingTop();
    float new_width = 0.0;
    float new_height = 0.0;
    for(auto it=_children.rbegin(); it != _children.rend(); it++)
    {
        auto ch = *it;
        
        ch->setX(chx);
        ch->setY(chy);
        chy += ch->height() + spacing();
        
        new_height += ch->height();
        if(ch->width() > new_width) new_width = ch->width();
    }
    new_width += (paddingLeft() + paddingRight());
    new_height += (paddingTop() + paddingBottom());
    new_height += (childrenCount() - 1) * spacing();
    resize(new_width, new_height);
}
    
}//namespace r64fx