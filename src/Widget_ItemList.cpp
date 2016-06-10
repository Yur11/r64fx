#include "Widget_ItemList.hpp"
#include "Painter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_ItemList::Widget_ItemList(Widget* parent)
: Widget_DataItem("", Widget_DataItem::Kind::List, parent)
{

}

}//namespace r64fx