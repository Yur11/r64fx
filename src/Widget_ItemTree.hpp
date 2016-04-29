#ifndef R64FX_WIDGET_ITEM_TREE_HPP
#define R64FX_WIDGET_ITEM_TREE_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ItemTree : public Widget{
public:
    Widget_ItemTree(Widget* parent = nullptr);

protected:
    virtual void updateEvent(UpdateEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_ITEM_TREE_HPP