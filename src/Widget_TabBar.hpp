#ifndef R64FX_WIDGET_TAB_BAR_HPP
#define R64FX_WIDGET_TAB_BAR_HPP

#include "Widget_Label.hpp"

namespace r64fx{
    
class Widget_Tab : public Widget_Label{
public:
    using Widget_Label::Widget_Label;
};
    
    
class Widget_TabBar : public Widget{
public:
    Widget_TabBar(Widget* widget = nullptr);
    
    virtual ~Widget_TabBar();
    
    void addTab(Widget_Tab* tab);
};
    
}//namespace r64fx

#endif//R64FX_WIDGET_TAB_BAR_HPP
