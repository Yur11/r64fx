#ifndef R64FX_WIDGET_ITEM_BROWSER_HPP
#define R64FX_WIDGET_ITEM_BROWSER_HPP

#include "Widget_ScrollArea.hpp"

namespace r64fx{

class Widget_DataItem;

class Widget_ItemBrowser : public Widget_ScrollArea{
    Widget_DataItem* m_root_item = nullptr;

public:
    Widget_ItemBrowser(Widget* parent = nullptr);

    void setRootItem(Widget_DataItem* item);

    Widget_DataItem* rootItem();

protected:
    virtual void paintEvent(Widget::PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_ITEM_BROWSER_HPP