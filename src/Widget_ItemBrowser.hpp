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

    void addItem(Widget_DataItem* item);

    void rearrange();

    void scrollTo(float position);

protected:
    virtual void paintEvent(Widget::PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_ITEM_BROWSER_HPP