#ifndef R64FX_WIDGET_DATA_ITEM_HPP
#define R64FX_WIDGET_DATA_ITEM_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_DataItem : public Widget{
    std::string m_text = "";

public:
    Widget_DataItem(Widget* parent = nullptr);

    void setText(const std::string &text);

    std::string text() const;

protected:
    virtual void updateEvent(UpdateEvent* event);
};

}//namespace

#endif//R64FX_WIDGET_DATA_ITEM_HPP