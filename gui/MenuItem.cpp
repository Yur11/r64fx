#include "MenuItem.h"
#include <iostream>

using namespace std;

namespace r64fx{

MenuItem::MenuItem(Icon* icon, TextLine* caption, TextLine* shortcut, Widget* parent)
: HorizontalContainer(parent)
{
    appendWidget(icon);    
    appendWidget(caption);
    appendWidget(shortcut);
    
    update();
}


void MenuItem::mousePressEvent(MouseEvent* event)
{
    on_click.send(this);
}

}//namespace r64fx
