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
        Plain,
        List,
        Tree
    };

    Widget_DataItem(const std::string &caption, Widget* parent = nullptr);

    Widget_DataItem(Widget* parent = nullptr);

    virtual ~Widget_DataItem();

    void setCaption(const std::string &caption);

    std::string caption() const;

    void setKind(Widget_DataItem::Kind kind);

    Widget_DataItem::Kind kind();

    int lineHeight();

    void addItem(Widget_DataItem* item);

    void addItem(const std::string &caption);

    void resizeAndReallign(int min_width);

private:
    void resizeAndReallignPlain(int min_width);

    void resizeAndReallignList(int min_width);

    void resizeAndReallignTree(int min_width);

public:
    int enumerate(int num);

private:
    int enumeratePlain(int num);

    int enumerateList(int num);

    int enumerateTree(int num);

public:
    Widget_DataItem* parentDataItem();

    Widget_DataItem* rootDataItem();

    Widget_ItemBrowser* parentBrowser();

    int indent() const;

    int indentWidth() const;

    void setSelected(bool yes);

    bool isSelected() const;

    virtual void collapse();

    virtual void expand();

    bool isExpanded();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void clipEvent(ClipEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

private:
    void mousePressEventPlain(MousePressEvent* event);

    void mousePressEventTree(MousePressEvent* event);

protected:
    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

    virtual void dndFinishedEvent(DndFinishedEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_DATA_ITEM_HPP