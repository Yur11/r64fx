#ifndef R64FX_WIDGET_DATA_ITEM_HPP
#define R64FX_WIDGET_DATA_ITEM_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ItemBrowser;

class Widget_DataItem : public Widget{
    std::string m_text = "";
    Image* m_image = nullptr;
    Rect<int> m_visible_rect;

public:
    Widget_DataItem(const std::string &text, Widget* parent = nullptr);

    Widget_DataItem(Widget* parent = nullptr);

    virtual ~Widget_DataItem();

    void setText(const std::string &text);

    std::string text() const;

    int lineHeight();

    virtual void resizeAndReallign(int min_width);

    virtual int enumerate(int num);

    Widget_DataItem* parentDataItem();

    Widget_DataItem* rootDataItem();

    Widget_ItemBrowser* parentBrowser();

    int indent() const;

    int indentWidth() const;

    void setSelected(bool yes);

    bool isSelected() const;

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void clipEvent(ClipEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

    virtual void dndFinishedEvent(DndFinishedEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_DATA_ITEM_HPP