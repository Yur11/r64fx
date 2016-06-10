#ifndef R64FX_WIDGET_ITEM_TREE_HPP
#define R64FX_WIDGET_ITEM_TREE_HPP

#include "Widget_DataItem.hpp"

namespace r64fx{

class Widget_ItemTree : public Widget_DataItem{

public:
    Widget_ItemTree(const std::string &caption, Widget* parent = nullptr);

    Widget_ItemTree(Widget* parent = nullptr);
};

}//namespace

#endif//R64FX_WIDGET_ITEM_TREE_HPP