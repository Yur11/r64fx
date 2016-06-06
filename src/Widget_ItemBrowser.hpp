#ifndef R64FX_WIDGET_ITEM_BROWSER_HPP
#define R64FX_WIDGET_ITEM_BROWSER_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ScrollArea;
class Widget_ScrollBar;
class Widget_DataItem;

class Widget_ItemBrowser : public Widget{
    friend class Widget_DataItem;
    Widget_DataItem* m_selected_item = nullptr;
    void (*m_on_item_selected)(Widget_ItemBrowser* browser, void* data) = nullptr;
    void *m_on_item_selected_data = nullptr;

public:
    Widget_ItemBrowser(Widget* parent = nullptr);

    virtual ~Widget_ItemBrowser();

    void addItem(Widget_DataItem* item);

    void rearrange();

    void scrollTo(float position);

    Widget_DataItem* selectedItem() const;

    void onItemSelected(void (*callback)(Widget_ItemBrowser* browser, void* data), void* data = nullptr);

protected:
    virtual void paintEvent(Widget::PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

private:
    void setSelectedItem(Widget_DataItem* item);
};

}//namespace r64fx

#endif//R64FX_WIDGET_ITEM_BROWSER_HPP