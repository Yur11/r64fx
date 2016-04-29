#include "Widget_ItemTree.hpp"
#include "Painter.hpp"

namespace r64fx{

Widget_ItemTree::Widget_ItemTree(Widget* parent)
: Widget(parent)
{

}


void Widget_ItemTree::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);
}

}//namespace