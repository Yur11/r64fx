#ifndef R64FX_WIDGET_ITEM_LIST_HPP
#define R64FX_WIDGET_ITEM_LIST_HPP

#include "Widget_DataItem.hpp"

namespace r64fx{

class Widget_ItemList : public Widget_DataItem{
public:
    Widget_ItemList(Widget* parent = nullptr);

protected:
    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_ITEM_LIST_HPP