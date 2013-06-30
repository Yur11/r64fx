#include "MenuItem.h"
#include <iostream>

using namespace std;

namespace r64fx{

MenuItem::MenuItem(Action* action, Widget* parent)
: Widget(parent)
{
}


void MenuItem::mousePressEvent(MouseEvent* event)
{
    action()->trigger();
}

}//namespace r64fx
