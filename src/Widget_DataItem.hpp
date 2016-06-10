#ifndef R64FX_WIDGET_DATA_ITEM_HPP
#define R64FX_WIDGET_DATA_ITEM_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_ItemBrowser;

class Widget_DataItem : public Widget{
    std::string m_caption = "";
    Image* m_image = nullptr;
    Rect<int> m_visible_rect;

public:
    enum class Kind{
        Text,
        List,
        Tree
    };

    Widget_DataItem(const std::string &caption, Widget_DataItem::Kind kind = Kind::Text, Widget* parent = nullptr);

    Widget_DataItem(Widget* parent = nullptr);

    virtual ~Widget_DataItem();

    void setCaption(const std::string &caption);

    std::string caption() const;

    Kind kind();

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

    void collapse();

    void expand();

    bool isCollapsed();

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