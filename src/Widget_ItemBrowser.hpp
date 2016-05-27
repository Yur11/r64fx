#ifndef R64FX_WIDGET_ITEM_BROWSER_HPP
#define R64FX_WIDGET_ITEM_BROWSER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ScrollArea;
class Widget_ScrollBar;
class Widget_DataItem;

class Widget_ItemBrowser : public Widget{
public:
    Widget_ItemBrowser(Widget* parent = nullptr);

    virtual ~Widget_ItemBrowser();

//     void setRootItem(Widget_DataItem* item);
//
//     Widget_DataItem* rootItem();

    Widget_ScrollArea* scrollArea();

    void showVerticalScrollBar(bool yes);

    Widget_ScrollBar* verticalScrollBar();

    void showHorizontalScrollBar(bool yes);

    Widget_ScrollBar* horizontalScrollBar();

protected:
    virtual void paintEvent(Widget::PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

private:
    void rearrange();
};

}//namespace r64fx

#endif//R64FX_WIDGET_ITEM_BROWSER_HPP