#ifndef R64FX_WIDGET_DATA_ITEM_HPP
#define R64FX_WIDGET_DATA_ITEM_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_DataItem : public Widget{
    std::string m_text = "";
    Image* m_image = nullptr;
    int m_tree_depth = 0;
    Rect<int> m_visible_rect;

public:
    Widget_DataItem(const std::string &text, Widget* parent = nullptr);

    Widget_DataItem(Widget* parent = nullptr);

    ~Widget_DataItem();

    void setText(const std::string &text);

    std::string text() const;

    int lineHeight();

    virtual void resizeAndReallign(int min_width);

    virtual int enumerate(int num);

    Widget_DataItem* parentDataItem();

    Widget_DataItem* rootDataItem();

    void setTreeDepth(int depth);

    int treeDepth() const;

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void clipEvent(ClipEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();
};

}//namespace

#endif//R64FX_WIDGET_DATA_ITEM_HPP