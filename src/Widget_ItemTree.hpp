#ifndef R64FX_WIDGET_ITEM_TREE_HPP
#define R64FX_WIDGET_ITEM_TREE_HPP

#include "Widget_DataItem.hpp"

namespace r64fx{

class Widget_ItemTree : public Widget_DataItem{
public:
    Widget_ItemTree(Widget* parent = nullptr);

    void addItem(Widget_DataItem* item);

protected:
    virtual void updateEvent(UpdateEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_ITEM_TREE_HPP