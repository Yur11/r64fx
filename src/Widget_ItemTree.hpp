#ifndef R64FX_WIDGET_ITEM_TREE_HPP
#define R64FX_WIDGET_ITEM_TREE_HPP

#include "Widget_DataItem.hpp"

namespace r64fx{

class Widget_ItemTree : public Widget_DataItem{

public:
    Widget_ItemTree(const std::string &text, Widget* parent = nullptr);

    Widget_ItemTree(Widget* parent = nullptr);

    void addItem(Widget_DataItem* item);

    void addItem(const std::string &text);

    virtual void resizeAndReallign();

    void collapse();

    void expand();

    bool isCollapsed();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_ITEM_TREE_HPP