#include "Widget_Container.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
Widget_Container::Widget_Container(Widget* parent)
:Widget(parent)
{
}


void Widget_Container::alignVertically()
{
    int max_width = 0;
    int running_height = paddingTop();

    for(auto w=begin(); w!=end(); w++)
    {
        w->setPosition({paddingLeft(), running_height});
        if(w->width() > max_width)
            max_width = w->width();
        running_height += w->height() + spacing();
    }

    setSize({
        max_width + paddingLeft() + paddingRight(),
        running_height - spacing() + paddingBottom()
    });
}


void Widget_Container::alignHorizontally()
{
    int max_hight = 0;
    int running_width = paddingLeft();

    for(auto w=begin(); w!=end(); w++)
    {
        w->setPosition({running_width, paddingTop()});
        if(w->height() > max_hight)
            max_hight = w->height();
        running_width += w->width() + spacing();
    }

    setSize({
        running_width - spacing() + paddingRight(),
        max_hight + paddingTop() + paddingBottom()
    });
}
 
}//namespace r64fx