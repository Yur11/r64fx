#ifndef R64FX_WIDGET_DATA_ITEM_HPP
#define R64FX_WIDGET_DATA_ITEM_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_DataItem : public Widget{
    std::string m_text = "";
    Image* m_image = nullptr;

public:
    Widget_DataItem(const std::string &text, Widget* parent = nullptr);

    Widget_DataItem(Widget* parent = nullptr);

    ~Widget_DataItem();

    void setText(const std::string &text);

    std::string text() const;

    virtual void resizeAndReallign();

    Widget_DataItem* parentDataItem();

    Widget_DataItem* rootDataItem();

protected:
    virtual void updateEvent(UpdateEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();
};

}//namespace

#endif//R64FX_WIDGET_DATA_ITEM_HPP